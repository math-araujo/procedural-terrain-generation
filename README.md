# Procedural Terrain Generation

[![Windows](https://github.com/math-araujo/procedural-terrain-generation/actions/workflows/Windows.yml/badge.svg?event=push)](https://github.com/math-araujo/procedural-terrain-generation/actions/workflows/Windows.yml) [![Ubuntu](https://github.com/math-araujo/procedural-terrain-generation/actions/workflows/Ubuntu.yml/badge.svg?event=push)](https://github.com/math-araujo/procedural-terrain-generation/actions/workflows/Ubuntu.yml)

| |
| :---: 
| ![Scene](docs/gifs/main.gif?raw=True) <br/> GIF showing the procedural terrain, water reflection and skybox |


This project is a C++20/OpenGL 4.5 implementation of a procedural terrain generator on GPU using coherent noise. It uses compute shaders to create heightmaps based on 2D fractal Brownian noise (fBm). To calculate normal vectors based on the terrain height, it uses compute shaders that leverages the shared memory of GPU blocks to apply the Sobel operator on the heightmap. The heightmap is then used by tessellation shaders to generate a triangulation for the terrain, with dynamic level-of-detail (LOD) based on the camera distance. Since the triangles are generated dynamically on the GPU, instead of precomputed UV-coordinates for each triangle vertex, triplanar texture mapping is used to assign texture coordinates on-the-fly. The reflections on the water are created using planar reflection, and textures are used to simulate distortions on the water surface. Instead of a classic distance-based fog, a halfspace fog is implemented, which uses an arbitrary plane to cut the space in two halfspaces: one affected and one unaffected by fog.

Check the [Gallery section on the GALLERY.md file](https://github.com/math-araujo/procedural-terrain-generation/blob/master/GALLERY.md) for more images/GIFs and the [Build section](https://github.com/math-araujo/procedural-terrain-generation#build) for instructions on how to build the project.

Tested on:

1) Ubuntu 20.04 + GCC-9, using Intel HD Graphics 520 (SKL GT2)

2) Windows 11 + MSVC 17, using NVIDIA RTX 3050 Laptop

## Features

* Procedural terrain generation on GPU using fractal Brownian noise (fBm)

* Dynamic level-of-detail (LOD) based on distance using tessellation shaders

* Heightmap generation on GPU using compute shaders

* Normal map generation on GPU using compute shaders and shared memory

* Water planar reflection

* Triplanar texture mapping

* Halfspace Fog

* Height-based and slope-based texturing

* Normal mapping

* Heightmap and normal map generation on CPU, including Piecewise Cubic Hermite Curves to adjust heights distributions (deprecated in favor of GPU approach, but the code remains available for reference)

## Build


### Assets License and Download

Unfortunately, some of the textures used in this project cannot be directly redistributed on this repository (or any other file sharing tool) due to licensing; they are free to download and use, but cannot be redistributed outside the original website. 

* Option 1: manually download the textures

In case anyone wants to manually download it, the file `assets/textures/source_textures.txt` contains the links to download. If you extract the files on the correct directories (normal maps into `assets/textures/terrain/normal`, albedos into `assets/textures/terrain/albedo` and ambient occlusion maps into `assets/textures/terrain/ao`), everything should work as expected.

* Option 2: automated script

Alternatively, if you have Python 3.9 and some dependencies installed (`requests`, and optionally `Pillow` in case you want to resize the textures), you can run a provided Python script as follows, with the terminal on the root of this repository:

```
python scripts/prepare_assets.py [--target_size]
```

, which will download the files from the site, optionally resizes the textures (if a value is provided to the argument `--target_size`) and save the files on the directory `assets/textures/terrain`. Notice that this requires an active Internet connection. The bottleneck is the download part, which may be faster or slower depending on your network speed. On my machine the entire script takes around one minute to complete, of which 58 seconds were due to the download.

After that, you can proceed to build the C++/OpenGL code, as described below.

### Build Instructions

This project uses CMake as build system generator. It's necessary to have it installed on your system. With the terminal open on the root directory of this project, execute the following commands on the command-line to configure and build the project:

```

# On Windows + MSVC
cmake --preset=default

cmake --build build --config Release

# On Unix
cmake --preset=default-unix

cmake --build build

```

## Controls

* A/S/D/W Key - Move left/backward/right/forward

* Q/E Key - Move down/up

* F Key - Switch between free FPS movement and locked FPS movement

* P Key - Switch between polygon mode and wireframe mode

* ESC Key - Exit application

* Left Mouse Button - Hold and move mouse to look-around when in locked FPS mode


## LICENSE

All the code is released under MIT License. The code on the file `assets/shaders/heightmap/noise.glsl` is due to Stefan Gustavson and is also released under a MIT License.

The cubemap textures (stored at `assets/textures/cubemap/` and `assets/textures/cloudy_cubemap`) are released under the [Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License](https://creativecommons.org/licenses/by-nc-sa/3.0/). The original cubemap textures, which you can access at [this link](https://reije081.home.xs4all.nl/skyboxes/), are provided at the Roel z'n Boel site and were release under the CC-BY-NC-SA License. My modifications to the original texture consisted on cropping the cubemap faces and resizing each cropped image. 

Check the LICENSE file on this repository for details.