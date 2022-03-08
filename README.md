# Magnum Dynamics
This is a small wrapper around magnum graphics to easily load/create objects and move them within the scene. Suitable to draw simple mechanical motions or function-based colored meshes.

### Authors/Maintainers
- Bernardo Fichera (bernardo.fichera@epfl.ch)

### ToDo
- Move prior transformation within the object
- Unify Object and DrawableObject
- Fix draw when material is present
- Fix set of mesh, color, material etc
- Remove debug messages
- Fix vertical rotation (problem when camera not in y=0, it keeps rotating about old y)

### Dependencies
This library depends on *Corrade*
```sh
git clone https://github.com/mosra/corrade.git (git@github.com:mosra/corrade.git)
cd corrade && mkdir build && cmake .. && make && (sudo) make install
```
and *Eigen*
```sh
git clone https://gitlab.com/libeigen/eigen.git (git@gitlab.com:libeigen/eigen.git)
cd eigen && mkdir build && cmake .. && (sudo) make install
```

### Installation
Clone the repository including the submodules
```sh
git clone --recursive https://github.com/nash169/graphics-lib.git (git@github.com:nash169/graphics-lib.git)
```
**graphics-lib** relies on WAF compilation tool.
Arch provides an updated version of WAF exec in the standard repo
```sh
sudo pacman -S waf
```
For other distros it is better to download the latest version from the official website and move the executable in the library repo
```sh
wget 'https://waf.io/waf-2.0.23'
mv waf-2.0.23 waf && mv waf /path/to/graphics-lib
cd /path/to/kernel-lib
chmod +x waf
```
Compile and install using waf commands
```sh
waf (./waf) configure build
```
or
```sh
waf (./waf) configure && waf (./waf)
```
Install the library (optional)
```sh
(sudo) waf (./waf) install
```
If you want to make a clean installation
```sh
(sudo) waf (./waf) distclean configure build install
```

#### Compilation options
In order to set the desired compiler define the environment variable CXX=<g++,clang++,icpc> (gnu, clang and intel compiler respectively).

It is highly recommended to compile with AVX support
```sh
waf (./waf) configure --release
```
Compile static library (default option)
```sh
waf (./waf) configure --static
```
Compile shared library
```sh
waf (./waf) configure --shared
```
Define a specific installation path
```sh
waf (./waf) configure --prefix=/path/to/install/folder
```

### Finding the library
In order to find and link the lib to other projects copy and paste the following file into the waf tools
```sh
scripts/graphicslib.py
```

### Examples
Once the library is compiled all the examples can be found in
```sh
./build/src/examples/
```
Certain examples depend on **utils-lib**
```sh
cd /path/to/utils-lib
waf (./waf) configure --release build install
```
