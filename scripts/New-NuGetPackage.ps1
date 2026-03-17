[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$OutputDirectory,

    [string]$ProjectPath = "src/Annoy.Net/Annoy.Net.csproj",

    [string]$PackageVersion,

    [string]$WindowsLibrary,

    [string]$LinuxLibrary,

    [string]$MacLibrary
)

$ErrorActionPreference = "Stop"

function Copy-NativeLibrary {
    param(
        [string]$SourcePath,
        [Parameter(Mandatory = $true)]
        [string]$DestinationPath
    )

    if ([string]::IsNullOrWhiteSpace($SourcePath)) {
        return $null
    }

    if (-not (Test-Path -LiteralPath $SourcePath)) {
        throw "Native library not found: $SourcePath"
    }

    $destinationDirectory = Split-Path -Parent $DestinationPath
    New-Item -ItemType Directory -Path $destinationDirectory -Force | Out-Null
    Copy-Item -LiteralPath $SourcePath -Destination $DestinationPath -Force
    return $DestinationPath
}

$resolvedProjectPath = (Resolve-Path $ProjectPath).Path
$resolvedOutputDirectory = if ([System.IO.Path]::IsPathRooted($OutputDirectory)) {
    [System.IO.Path]::GetFullPath($OutputDirectory)
}
else {
    [System.IO.Path]::GetFullPath((Join-Path (Get-Location) $OutputDirectory))
}

$tempRoot = Join-Path ([System.IO.Path]::GetTempPath()) ("annoy-nuget-" + [Guid]::NewGuid().ToString("N"))

try {
    $windowsPath = Copy-NativeLibrary -SourcePath $WindowsLibrary -DestinationPath (Join-Path $tempRoot "win-x64\annoy_c.dll")
    $linuxPath = Copy-NativeLibrary -SourcePath $LinuxLibrary -DestinationPath (Join-Path $tempRoot "linux-x64\libannoy_c.so")
    $macPath = Copy-NativeLibrary -SourcePath $MacLibrary -DestinationPath (Join-Path $tempRoot "osx-x64\libannoy_c.dylib")

    New-Item -ItemType Directory -Path $resolvedOutputDirectory -Force | Out-Null

    $arguments = @(
        "pack",
        $resolvedProjectPath,
        "-c", "Release",
        "-o", $resolvedOutputDirectory,
        "-p:ContinuousIntegrationBuild=true"
    )

    if (-not [string]::IsNullOrWhiteSpace($PackageVersion)) {
        $arguments += "-p:PackageVersion=$PackageVersion"
    }

    if ($windowsPath) {
        $arguments += "-p:AnnoyNativeWindowsPath=$windowsPath"
    }

    if ($linuxPath) {
        $arguments += "-p:AnnoyNativeLinuxPath=$linuxPath"
    }

    if ($macPath) {
        $arguments += "-p:AnnoyNativeMacPath=$macPath"
    }

    & dotnet @arguments
}
finally {
    if (Test-Path -LiteralPath $tempRoot) {
        Remove-Item -LiteralPath $tempRoot -Recurse -Force
    }
}
