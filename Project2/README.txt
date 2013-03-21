Things to mention in the presentation
=====================================

Meshes created by the define<Shape> functions do not create duplicate vertices
	Smooth normals calculated as vertices defined = less memory and no extra normals calculation after initialization
	Down side: No duplicates w/ flat normals = incorrect flat shadding. However, we have enough faces that it isn't noticeable. Yay cheating!
	Circle and Rhombus overlap vertices on edges so we can get sharp edges even with smooth shading
	Since shapes are all constructed the same way (row-major order, excluding the circle) we were able to make a function to push the vertex indices
	Didn't realize I could just use a matrix and use glm:rotate()... so did it the hard way and implemented function to create a rotation matrix for me

Shaders
	ADS calculations: use light properties but not material properties (just use color instead)
	Pattern shader: bricks - Color based off of object material
	Attenuation: Phong shader with attenuation. Uses same vertex shader as basic Phong because there is no difference there
	Many failed attempts trying to pass a shader to the objects, settled on passing shader pointer into draw method - only create each shader once this way

Objects
	Axes and Grid from P1 turned into objects to make debugging while working on meshes easier
	Normals, Points, Wireframe also useful when working on meshes
	2 camera modes: Target rotation (from P1) and free rotation (camera rotates like in a FPS)
	Move camera up/down/forwards/backwards/right/left
	Zoom camera in/out
	F1 cycles through objects
	Made list of objects in main.cpp so that we can take care of inits and takedowns of all objects in just a few lines