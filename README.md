# Annoy.NET 
*Annoy.NET* is a C# Wrapper for [Spotify's Annoy](https://github.com/spotify/annoy), it can be used either with Unity or your C# based Application.

## Unity Integration
1. Drag & Drop `annoy.net.unitypackage` into unity editor
2. Profit!

## Usage
```csharp
using System;  
using UnityEngine;  
using AnnoyWrapper;  
  
public class Test : MonoBehaviour  
{  
    void Start()  
    {
        IntPtr index = Annoy.Index(512, AnnoyMetric.Angular);  
        Annoy.AddItem(index, 0, new float[512]);  
        Annoy.AddItem(index, 1, new float[512]);  
        Annoy.AddItem(index, 2, new float[512]);  
        Annoy.Build(index, 10);  
        Debug.Log(Annoy.GetNItems(index)); // 3  
        Annoy.Free(index);  
    }}
```

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

3. Build the native library using CMake (works on Windows, Linux, and other platforms):

```sh
cmake -S annoy_clang -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

   On Linux this generates `libannoy_c.so`, while on Windows it produces `annoy_c.dll`.

## TODO
* Metric_Hamming: add support to C-wrapper and P/Invoke.
* Cross-platform compatibility: builds for macOS (.dylib)

## License
This project includes and wraps [Spotify Annoy](https://github.com/spotify/annoy), a C++ library for approximate nearest neighbors.
