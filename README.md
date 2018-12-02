# Spkgen
Spkgen is an editor for the [**SPARK**](https://github.com/fredakilla/SPARK) particles engine using a node-based interface to create particles effects for games.
 
## Screenshots
<img src="https://i.imgur.com/4uplO1O.png" width="50%" height="%">

## Current status
Spkgen is in developpement and is not yet fully functional.
Development is done on linux.
Building on another system is not tested but final release should be cross-platform.


## Features
- C++ and Qt based interface.
- Spkgen internally use [**Urho3D**](https://github.com/urho3d/Urho3D) for rendering particles.
- and [**Urhox**](https://github.com/fredakilla/Urhox) library to bind the SPARK particle engine to Urho3D.
- node core system is provided by the external qt [**nodeeditor**](https://github.com/paceholder/nodeeditor) library.

TODO
- [x] implement generic nodes classes
- [x] automatic node's parameters widgets creation from simple declaration
- [ ] show 3D debug zones geometries
- [x] serialize node widget's parameters
- [x] graph plot editor widget for spark graph interpolators (WIP but maybe do this using imgui)
- [ ] complete all spark nodes (95% done)
- [ ] interact with 3D view (camera move...) + use the in-game editor
- [ ] multiple scenes managment
- [ ] possibility to attach result effect to scene nodes (static node, node in motion, trow bullet, ...)
- [ ] ~~add pliable parameters button to show/hide parameters widgets~~
- [x] comments items on node view
- [ ] set styles
- [ ] add shortcuts on node view : copy/paste nodes, repeat last created node,...

## Build

Spkgen developpement is done with Qt 5.11.1 + QtCharts module for tgraph plot editor widget.   
WARNING : Do not use Qt 5.11.2, I experienced some bugs with FileDialogs on linux...


##### 1. Get submodules
```
git submodule init
git submodule update
```

##### 2. Build the external nodeeditor Qt library
```
cd 3rdparty/nodeeditor
mkdir BUILD
cd BUILD
make
```

##### 3. Set URHO3D_HOME and URHOX_HOME variable path in QtCreator :
(Urho3D and Urhox libraries need to be builded)
```bash
- Open spkgen.pro with QtCreator.
- Edit spkgen.pro file
- Set variable path :

#--------------------------------------------------------------------
# path to Urho3D and Urhox
#--------------------------------------------------------------------
URHO3D_HOME = /home/John/Dev/Urho3D/BUILD
URHOX_HOME = /home/John/Dev/Urhox/output
```

##### 4. Build editor
```
- Open spkgen.pro with QtCreator.  
- Build and Run.
```