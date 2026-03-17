# Annoy.NET
*Annoy.NET* is a cross-platform C# wrapper for [Spotify's Annoy](https://github.com/spotify/annoy). It can be used from Unity and from regular .NET applications through the `Annoy.Net` NuGet package.

## Unity
1. Download `annoy.net.unitypackage` from the GitHub release assets.
2. Import it into your Unity project.
3. Unity will install the wrapper under `Assets/Annoy.NET`.
4. The release `unitypackage` includes the native plugins for Windows, Linux, and ~~macOS~~.

## NuGet
Install the managed wrapper and the platform-specific native library with:

```sh
dotnet add package Annoy.Net
```

The NuGet package ships native assets for `win-x64`, `linux-x64`, and ~~`osx-x64`~~.

## Usage
```csharp
using System;  
using UnityEngine;  
using AnnoyWrapper;  
  
public class Test : MonoBehaviour  
{  
    void Start()  
    {
        using (var index = Annoy.Index(512, AnnoyMetric.Angular))
        {
            Annoy.AddItem(index, 0, new float[512]);  
            Annoy.AddItem(index, 1, new float[512]);  
            Annoy.AddItem(index, 2, new float[512]);  
            Annoy.Build(index, 10);  
            Debug.Log(Annoy.GetNItems(index)); // 3  
        }
    }}
```

## Project Layout
- `dev.trainhead.annoy.net/` contains the Unity-facing package content, samples, and Unity runtime tests.
- `src/Annoy.Net/Annoy.Net.csproj` exposes the same managed API for non-Unity consumers and produces the `Annoy.Net` NuGet package.
- `tests/Annoy.ManagedSmoke/` contains a cross-platform .NET smoke test that runs against the native library.
- `tests/Annoy.NuGetSmoke/` validates the packaged `.nupkg` through a real `PackageReference`.

## Setup & Build Instructions
1. Clone this repository

```sh
git clone https://github.com/TrainHead-software/annoy.net.git
cd annoy.net
```

2. Clone the Annoy source inside project folder

```sh
git clone https://github.com/spotify/annoy.git annoy
```

3. Build the native library using CMake:

```sh
cmake -S annoy_clang -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

   This produces `annoy_c.dll` on Windows, `libannoy_c.so` on Linux ~~, and `libannoy_c.dylib` on macOS~~.

4. Run the native and managed smoke tests:

```sh
ctest --test-dir build --output-on-failure
dotnet run --project tests/Annoy.ManagedSmoke/Annoy.ManagedSmoke.csproj
```

5. Build the Unity package locally:

```sh
pwsh ./scripts/New-UnityPackage.ps1 -OutputPath ./dist/annoy.net.unitypackage -WindowsLibrary ./build/Release/annoy_c.dll
```

   Add `-LinuxLibrary` and `-MacLibrary` when those binaries are available locally. In GitHub Actions, the release workflow injects all native libraries automatically before publishing `annoy.net.unitypackage`.

6. Build the NuGet package locally:

```sh
pwsh ./scripts/New-NuGetPackage.ps1 -OutputDirectory ./dist/nuget -WindowsLibrary ./build/Release/annoy_c.dll
```

   Add `-LinuxLibrary` and `-MacLibrary` when those binaries are available locally.

## License
This project includes and wraps [Spotify Annoy](https://github.com/spotify/annoy), a C++ library for approximate nearest neighbors.
