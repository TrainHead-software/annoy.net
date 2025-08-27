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
git clone github.com/TrainHead-software/annoy.net
cd annoy.net
```

2. Clone the Annoy source inside project folder

```sh
git clone https://github.com/spotify/annoy.git
```

3. Build the DLL Using CMake (MinGW, Ninja, or Visual Studio):

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

## TODO
* Metric_Hamming: add support to C-wrapper and P/Invoke.
* Cross-platform compatibility: builds for Linux/macOS (.so/.dylib)

## License
This project includes and wraps [Spotify Annoy](https://github.com/spotify/annoy), a C++ library for approximate nearest neighbors.