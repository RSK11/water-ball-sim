# Water Balloon Simulator


Water Balloon Simulator was my final project for my Introduction to Computer Graphics class. The goal of the project was to implement a soft body physics simulation with simple collision detection. The technique used was inspired by the paper "Meshless Deformations Based on Shape Matching" by Matthias Mueller.


###### I was responsible for the following:

[src/main](src/main.cpp)

[src/Material](src/Material.cpp)

[src/TPCamera](src/TPCamera.cpp)

[src/WBalloon](src/WBalloon.cpp)

[src/WObject](src/WObject.cpp)

[resources/frag](resources/frag.glsl)

[resources/vert](resources/vert.glsl)


The rest of the code was provided by the instructor.




## ORIGINAL README:


Compile and Run using:


mkdir build

cd build

cmake ..

make

./Final ../resources


or


./Final ../resources horizontalSlices[1+] verticalSlices[6+] r[0-255] g[0-255] b[0-255]



References:


Wikipedia - For procedurally drawing a sphere

https://en.wikipedia.org/wiki/Spherical_coordinate_system

https://en.wikipedia.org/wiki/List_of_common_coordinate_transformations#From_spherical_coordinates


Stack Overflow - For figuring out how to calculate the vertex normals

http://stackoverflow.com/questions/16340931/calculating-vertex-normals-of-a-mesh


Meshless Deformations Based on Shape Matching - For the sphere physics

http://matthias-mueller-fischer.ch/publications/MeshlessDeformations_SIG05.pdf


Texturelib - For the grass texture

http://texturelib.com/Textures/grass/grass/grass_grass_0071_02_preview.jpg


Textures101.com - For the box texture

http://textures101.com/textures/Wood/Planks/2011/7/18/tn1_seamless_9_bcluz.jpg
