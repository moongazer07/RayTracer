# Raytracer

With this repository you can build quite an effective version of raytracer that I created.

## How to make?

You will need to have libpng on your machine downloaded in order to build the project:

For Linux use `sudo apt-get install libpng-dev`

For Windows use `./vcpkg install libpng`

Once you have the library you can cmake with the following command:

For Debug: `cmake -DCMAKE_BUILD_TYPE=Debug -H. -Bbuild/Debug`
For Release: `cmake -DCMAKE_BUILD_TYPE=Release -H. -Bbuild/Release`

Release works way faster than debug, so `cd build/Release`, than do `make` and `cd bin/`.

At this moment you have reached the binary folder, where you have the binary for raytracer.

## How to run?

The binary takes as input up to two arguments: an absolute path to an .obj file that you want to render, and an optional argument -BVH. BVH flag is used to increase the speed of raytracer, using box volumes for objects. 




