# Spkgen
Spkgen is an editor for the [**SPARK**](https://github.com/fredakilla/SPARK) particles engine using a node-based interface to create particles effects for games.
 
## Screenshots
<img src="https://i.imgur.com/6BVsIWL.png" width="50%" height="%">


## Current status
Spkgen is in developpement and is not yet fully functional.  
Development is done on linux.  
Building on another system is not tested but final release should be cross-platform.


## Features
- C++ and Qt based interface.
- Spkgen internally use the [**gplay 3D engine**](https://github.com/fredakilla/GPlayEngine) for rendering particles and bind the SPARK particle engine.
- node core system is provided by the external [**nodeeditor**](https://github.com/paceholder/nodeeditor) library.

TODO
- [x] implement base nodes classes
- [x] show 3D debug zones geometries
- [x] serialize node widget's parameters
- [ ] add +,- buttons to add/remove dynamic inputs on some nodes using lists (emitters, groups)
- [ ] graph plot editor widget for spark graph interpolators
- [ ] complete all spark nodes
- [ ] interact with 3D view (camera move...)
- [ ] multiple scenes managment
- [ ] possibility to attach result effect to scene nodes (static node, node in motion, trow bullet, ...)
- [ ] add pliable parameters button to show/hide parameters widgets
- [ ] comments items on node view
- [ ] set styles


## Build

##### 1. Place the Spkgen directory at same top level of gplay engine :
```bash
├── YourDirectory
│	├── GPlayEngine
│	├── spkgen
```

##### 2. Get submodules
```
git submodule init
git submodule update
```

##### 3. Build the external nodeeditor library
```
cd 3rdparty/nodeeditor
mkdir BUILD
cd BUILD
make
```

##### 4. Build editor
```
- Open spkgen.pro with QtCreator.  
- Build and Run.
```