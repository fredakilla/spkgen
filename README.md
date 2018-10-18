# spkgen
spkgen is an editor for the SPARK particles engine using a node-based interface to create particles effects for games.

## Screenshots
<img src="https://i.imgur.com/6BVsIWL.png" width="50%" height="%">


## Current status
spkgen is in developpement and is not yet fully functional.


## Features
- c++ and Qt based interface.
- spkgen internally use the gplay 3D engine for rendering particles and bind the SPARK particle engine.
- node core system is provided by the external nodeeditor library.


## Build
###step 0: place the spkgen directory at same top level of gplay engine :

- yourDirectory
-- | GPlayEngine
-- | spkgen

###step 1: build nodeeditor library

cd 3rdparty/nodeeditor
mkdir BUILD
cd BUILD
make

###step 2: build editor

open spkgen.pro with QtCreator.
build and run.

