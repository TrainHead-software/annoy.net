# Annoy.NET — C API Wrapper for Spotify Annoy

## License
This project includes and wraps [Spotify Annoy](https://github.com/spotify/annoy), a C++ library for approximate nearest neighbors. 

## Setup & Build Instructions
1. Clone this repository
```
git clone github.com/TrainHead-software/annoy.net
cd annoy.net
```

2. Clone the Annoy source inside this repository
```
git clone https://github.com/spotify/annoy.git
⚠️ The annoy folder must be placed inside the root directory (next to annoy_clang).
```

3. Build the DLL
Using CMake (MinGW, Ninja, or Visual Studio):
```
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

## Unity Integration

1. Copy the compiled DLL into your Unity project:
```
YourUnityProject/
└── Assets/
    └── Plugins/
        └── x86_64/
            └── annoy_c.dll
```

2. Add the Annoy.cs P/Invoke wrapper to Assets/Scripts/.

3. Example usage:
```
IntPtr index = Annoy.Index(512, AnnoyMetric.Angular);
Annoy.AddItem(index, 0, new float[512]);
Annoy.Build(index, 10);
// ...
```
