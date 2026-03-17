[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,

    [string]$PackageRoot = "dev.trainhead.annoy.net",

    [string]$WindowsLibrary,

    [string]$LinuxLibrary,

    [string]$MacLibrary
)

$ErrorActionPreference = "Stop"

function Get-AssetGuid {
    param(
        [Parameter(Mandatory = $true)]
        [string]$MetaPath
    )

    $content = Get-Content -Raw $MetaPath
    $match = [System.Text.RegularExpressions.Regex]::Match($content, "(?m)^guid:\s*(\S+)\s*$")
    if (-not $match.Success) {
        throw "Missing guid in meta file: $MetaPath"
    }

    return $match.Groups[1].Value
}

function Add-UnityPackageEntry {
    param(
        [Parameter(Mandatory = $true)]
        [string]$PayloadRoot,

        [Parameter(Mandatory = $true)]
        [string]$MetaPath,

        [Parameter(Mandatory = $true)]
        [string]$Pathname,

        [string]$AssetPath
    )

    $guid = Get-AssetGuid -MetaPath $MetaPath
    $entryRoot = Join-Path $PayloadRoot $guid
    New-Item -ItemType Directory -Path $entryRoot -Force | Out-Null

    Set-Content -Path (Join-Path $entryRoot "pathname") -Value $Pathname -NoNewline
    Copy-Item -LiteralPath $MetaPath -Destination (Join-Path $entryRoot "asset.meta") -Force

    if ($AssetPath) {
        Copy-Item -LiteralPath $AssetPath -Destination (Join-Path $entryRoot "asset") -Force
    }
}

function Copy-NativeLibrary {
    param(
        [string]$SourcePath,
        [Parameter(Mandatory = $true)]
        [string]$DestinationPath
    )

    if ([string]::IsNullOrWhiteSpace($SourcePath)) {
        return
    }

    if (-not (Test-Path -LiteralPath $SourcePath)) {
        throw "Native library not found: $SourcePath"
    }

    $destinationDirectory = Split-Path -Parent $DestinationPath
    New-Item -ItemType Directory -Path $destinationDirectory -Force | Out-Null
    Copy-Item -LiteralPath $SourcePath -Destination $DestinationPath -Force
}

function Get-RelativePackagePath {
    param(
        [Parameter(Mandatory = $true)]
        [string]$BasePath,

        [Parameter(Mandatory = $true)]
        [string]$TargetPath
    )

    $resolvedBasePath = [System.IO.Path]::GetFullPath($BasePath)
    if (-not $resolvedBasePath.EndsWith([System.IO.Path]::DirectorySeparatorChar.ToString())) {
        $resolvedBasePath += [System.IO.Path]::DirectorySeparatorChar
    }

    $resolvedTargetPath = [System.IO.Path]::GetFullPath($TargetPath)
    $baseUri = New-Object System.Uri($resolvedBasePath)
    $targetUri = New-Object System.Uri($resolvedTargetPath)

    return [System.Uri]::UnescapeDataString($baseUri.MakeRelativeUri($targetUri).ToString())
}

$resolvedPackageRoot = (Resolve-Path $PackageRoot).Path
$resolvedOutputPath = if ([System.IO.Path]::IsPathRooted($OutputPath)) {
    [System.IO.Path]::GetFullPath($OutputPath)
}
else {
    [System.IO.Path]::GetFullPath((Join-Path (Get-Location) $OutputPath))
}
$tempRoot = Join-Path ([System.IO.Path]::GetTempPath()) ("annoy-unitypackage-" + [Guid]::NewGuid().ToString("N"))
$stagingRoot = Join-Path $tempRoot "stage"
$assetsRoot = Join-Path $stagingRoot "Assets"
$stagingPackageRoot = Join-Path $assetsRoot "Annoy.NET"
$payloadRoot = Join-Path $tempRoot "payload"

try {
    New-Item -ItemType Directory -Path $stagingPackageRoot -Force | Out-Null
    Copy-Item -Path (Join-Path $resolvedPackageRoot "*") -Destination $stagingPackageRoot -Recurse -Force

    Remove-Item -LiteralPath (Join-Path $stagingPackageRoot "package.json") -Force
    Remove-Item -LiteralPath (Join-Path $stagingPackageRoot "package.json.meta") -Force

    $samplesSource = Join-Path $stagingPackageRoot "Samples~"
    $samplesMetaSource = Join-Path $stagingPackageRoot "Samples~.meta"
    $samplesDestination = Join-Path $stagingPackageRoot "Samples"
    $samplesMetaDestination = Join-Path $stagingPackageRoot "Samples.meta"

    if (Test-Path -LiteralPath $samplesSource) {
        Move-Item -LiteralPath $samplesSource -Destination $samplesDestination
    }

    if (Test-Path -LiteralPath $samplesMetaSource) {
        Move-Item -LiteralPath $samplesMetaSource -Destination $samplesMetaDestination
    }

    Copy-NativeLibrary -SourcePath $WindowsLibrary -DestinationPath (Join-Path $stagingPackageRoot "Plugins\x86_64\annoy_c.dll")
    Copy-NativeLibrary -SourcePath $LinuxLibrary -DestinationPath (Join-Path $stagingPackageRoot "Plugins\x86_64\libannoy_c.so")
    Copy-NativeLibrary -SourcePath $MacLibrary -DestinationPath (Join-Path $stagingPackageRoot "Plugins\libannoy_c.dylib")

    New-Item -ItemType Directory -Path $payloadRoot -Force | Out-Null

    Get-ChildItem -Path $stagingPackageRoot -Directory -Recurse |
        Sort-Object FullName |
        ForEach-Object {
            $metaPath = $_.FullName + ".meta"
            if (-not (Test-Path -LiteralPath $metaPath)) {
                throw "Missing folder meta file: $metaPath"
            }

            $pathname = Get-RelativePackagePath -BasePath $stagingRoot -TargetPath $_.FullName
            Add-UnityPackageEntry -PayloadRoot $payloadRoot -MetaPath $metaPath -Pathname $pathname
        }

    Get-ChildItem -Path $stagingPackageRoot -File -Recurse |
        Where-Object { $_.Extension -ne ".meta" } |
        Sort-Object FullName |
        ForEach-Object {
            $metaPath = $_.FullName + ".meta"
            if (-not (Test-Path -LiteralPath $metaPath)) {
                throw "Missing asset meta file: $metaPath"
            }

            $pathname = Get-RelativePackagePath -BasePath $stagingRoot -TargetPath $_.FullName
            Add-UnityPackageEntry -PayloadRoot $payloadRoot -MetaPath $metaPath -Pathname $pathname -AssetPath $_.FullName
        }

    $outputDirectory = Split-Path -Parent $resolvedOutputPath
    if ($outputDirectory) {
        New-Item -ItemType Directory -Path $outputDirectory -Force | Out-Null
    }

    if (Test-Path -LiteralPath $resolvedOutputPath) {
        Remove-Item -LiteralPath $resolvedOutputPath -Force
    }

    & tar -czf $resolvedOutputPath -C $payloadRoot .
}
finally {
    if (Test-Path -LiteralPath $tempRoot) {
        Remove-Item -LiteralPath $tempRoot -Recurse -Force
    }
}
