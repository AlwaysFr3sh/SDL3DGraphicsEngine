# SDL3DGraphicsEngine
3D Graphics engine from scratch using SDL for window and dots :)

I will write a proper README when I have something that works, for now just know that I am writing this by following along with this: https://gabrielgambetta.com/computer-graphics-from-scratch/

For now also this is just rasterization. I don't think my mac can handle the Raytracing stuff

# BUGS

Weird artifacts in FilledTriangles, source is either FillTriangle() function, or the Interpolate() function

Shaded Triangles, straight up don't work proplerly, really weird shit going on, in the example,
the bottom half of the triangle is shaded perfectly, but the top bit is all wack. 

I am thinking maybe the bug is the result of some weird decimal bug(?), not completely sure, 
I am fairly confident it's something to do with when we compute the h-values inside the loop, which might
have something to do with the Interpolate function.

