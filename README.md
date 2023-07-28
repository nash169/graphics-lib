# Magnum Dynamics
This is a small wrapper around magnum graphics to easily load/create objects and move them within the scene. Suitable to draw simple mechanical motions or function-based colored meshes.

## Authors/Maintainers
- Bernardo Fichera (bernardo.fichera@epfl.ch)

## Available Utils
- primitive
- import from file

## ToDo
- Unify Object and DrawableObject
- Fix draw when material is present
- Fix set of mesh, color, material etc
- Remove debug messages
- Fix vertical rotation (problem when camera not in y=0, it keeps rotating about old y)

## Usage
Work in progress. Check the examples to get an idea.

## Dependencies
This library depends on **Corrade**
```sh
git clone https://github.com/mosra/corrade.git (git@github.com:mosra/corrade.git)
cd corrade && mkdir build && cmake .. && make && (sudo) make install
```
and **Eigen**
```sh
git clone https://gitlab.com/libeigen/eigen.git (git@gitlab.com:libeigen/eigen.git)
cd eigen && mkdir build && cmake .. && (sudo) make install
```

In addition, in order to compile the project, install my [waf-tools](https://github.com/nash169/waf-tools.git).

## Installation
Clone the repository
```sh
git clone https://github.com/nash169/graphics-lib.git (git@github.com:nash169/graphics-lib.git)
```
Compile and install using waf commands
```sh
waf configure build
```
or
```sh
waf configure && waf
```
Install the library (optional)
```sh
(sudo) waf install
```
If you want to make a clean installation
```sh
(sudo) waf distclean configure build install
```

### Compilation options
In order to set the desired compiler define the environment variable CXX=<g++,clang++,icpc> (gnu, clang and intel compiler respectively).

It is highly recommended to compile with AVX support
```sh
waf configure --release
```
Compile static library (default option)
```sh
waf configure --static
```
Compile shared library
```sh
waf configure --shared
```
Define a specific installation path
```sh
waf configure --prefix=/path/to/install/folder
```

## Examples
Once the library is compiled all the examples can be run with
```sh
./build/src/examples/<name_example>
```
