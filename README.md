# Procedural Terrain Generation

![Scene](docs/gifs/main.gif)

This project is a C++20/OpenGL 4.5 implementation of a procedural terrain generator on GPU using coherent noise. It uses compute shaders to create heightmaps based on 2D fractal Brownian noise (fBm). To calculate normal vectors based on the terrain height, it uses compute shaders that leverages the shared memory of GPU blocks to apply the Sobel operator on the heightmap. The heightmap is then used by tessellation shaders to generate a triangulation for the terrain, with dynamic level-of-detail (LOD) based on the camera distance. Since the triangles are generated dynamically on the GPU, instead of precomputed UV-coordinates for each triangle vertex, triplanar texture mapping is used to assign texture coordinates on-the-fly. The reflections on the water are created using planar reflection, and textures are used to simulate distortions on the water surface. Instead of a classic distance-based fog, a halfspace fog is implemented, which uses an arbitrary plane to cut the space in two halfspaces: one affected and one unaffected by fog.

Check the [Gallery section](https://github.com/math-araujo/procedural-terrain-generation#gallery) for more images/GIFs and the [Build section](https://github.com/math-araujo/procedural-terrain-generation#build) for instructions on how to build the project.

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

## Gallery

### Views

Some views of the scene

| |
| :---: 
| ![View2](docs/images/terrain_view2.png?raw=True) |

| |
| :---: 
| ![View1](docs/images/terrain_view1.png?raw=True) |



### Noise Settings

The coherent noise used was 2D fractal Brownian noise (fBm), which adds layers of noise with increasing frequency but decreasing influence. The first layers of noise defines the overall shape of the terrain, while the subsequent layers defines small-scale details of the terrain. The number of layers is denoted by octaves, the sampling frequency is controlled by the lacunarity variable and the influence of each layer is decided by the persistance variable. The following image show the real-time update of the terrain by changing these parameters on the GUI.

| |
| :---: |
| ![Noise Settings GIF](docs/gifs/noise.gif) <br/> Noise parameters that affect the terrain height and shape |

### Terrain Tessellation

![Terrain Tessellation](docs/images/tessellation.png?raw=True) <br/> Terrain tessellation generated by tessellation shaders. The level of detail changes dynamically based on the camera distance, creating more triangles for areas closer to the viewer.


### Water Reflection

| | |
| :---: | :---: |
| ![Water Surface Deformation](docs/images/water_ripples.png?raw=True) <br/> Waves on water surface | ![Water Reflection](docs/images/water_reflection.png?raw=True) <br/> Reflections on water surface |


### Normal Mapping

Normal mapping allows to increase the appearance of surface details without increasing the geometry (i.e. number of polygons).

| |
| :---: |
| ![Normal Mapping GIF](docs/gifs/normalmap.gif) <br/> GIF showing the increase in details on the terrain surface by enabling normal mapping |


The images below shown some images without (left) and with (right) normal mapping.


| | |
| :---: | :---: |
| ![Without Normal Mapping](docs/images/nonormalmap2.jpeg?raw=True) <br/> Without Normal Mapping | ![With Normal Mapping](docs/images/normalmap2.jpeg?raw=True) <br/> With Normal Mapping |
| ![Without Normal Mapping](docs/images/nonormalmap1.jpeg?raw=True) <br/> Without Normal Mapping | ![With Normal Mapping](docs/images/normalmap1.jpeg?raw=True) <br/> With Normal Mapping |


### Height-based Texture Blending

The terrain height determines which texture to sample from. To avoid abrupt changes of texture, a blending range is defined, where two textures are blended together. This blending range is determined using a height start and height end parameter for each texture, which creates different effects as desired. The corner cases are the first and last texture, which always start at 0.0 and end at 1.0, respectively. 

| | |
| :---: | :---: |
| ![BlendExample1](docs/images/blend1.png?raw=True) <br/> Default height-based texturing | ![BlendExample2](docs/images/blend2.png?raw=True) <br/> Increasing the River-Rock texture range |


### Triplanar Texture Mapping

In the images below it's possible to see the effect of a simpler texture mapping, planar mapping (left), compared to triplanar texture mapping (right). Vertical stretches are much more noticeable on the case of planar mapping, specially when the terrain elevation parameter is increased. Those stretches are considerably softned by using triplanar texture mapping. 

| | |
| :---: | :---: |
| ![Without Triplanar Texture Mapping](docs/images/projection.jpeg?raw=True) <br/> Without Triplanar Texture Mapping | ![With Triplanar Texture Mapping](docs/images/triplanar.jpeg?raw=True) <br/> With Triplanar Texture Mapping |


### Halfspace Fog

Halfspace fog splits the space in two halfspaces, one affected and one unaffected by fog. This allows to create some interesting visual effects, such as fog with linear density on mountain valleys.

| | |
| :---: | :---: |
| ![Fog1](docs/images/fog1.png?raw=True) <br/> Fog height = 20, density constant = 0.001 | ![Fog2](docs/images/fog2.png?raw=True) <br/> Fog height = 20, density constant = 0.1 |
| ![Fog3](docs/images/fog3.png?raw=True) <br/> Fog height = 30.149, density constant = 0.001 | ![Fog4](docs/images/fog4.png?raw=True) <br/> Fog height = 30.149, density constant = 0.011 |

While the usage on this project seems like a height-based fog, the algorithm of halfspace fog is flexible and allows to use an arbitrary plane, and not only an axis-aligned plane, to define regions affected and unaffected by fog. 

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