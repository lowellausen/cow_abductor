# cow_abductor

Simple game made entirely with with C++ and OpenGL/GLSL on Code::Blocks. The purpose of this project is to better understand the main concepts of computer graphics, such as: loading 3D models, rendering them, texture mapping, calculating colisions, positions, controlling cameras, geometrical transforms, lighting, shading.

OpenGL was used mostly to send the 3D object's nodes to the GPU, all geometrical transforms (model, view and projection matrix, their compositions and transformations) were made by hand for better understanding. All shader where also programmed by hand with GLSL.

## the game

The game is pretty simple and has really no goal. A UFO showed up in the farm and now all the cows are running to the barn to save their lives. You are the UFO and you can abduct the cows before they reach the barn! but be careful not to run over them, they could get hurt!

###
Controls: 
WASD to move the UFO around
Right mouse click and drag to rotate camera
Space bar to abduct a cow (when it's inside the light cone)
Z and X to move the UFO up and down

### Screenshots

Initial scene with cows running to the barn

![Alt text](data/screenshots/scene.png?raw=true "Initial scene")

Abducting a cow

![Alt text](data/screenshots/abducting.png?raw=true "Abducting")

Murdering a cow

![Alt text](data/screenshots/dead_cow.png?raw=true "Dead Cow")
