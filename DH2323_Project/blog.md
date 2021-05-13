# Pontus Asp's blog for DH2323 Project
In this blog I will take notes of my progress and thought when working on the DH2323 Project. In this project I am going to program a fluid simulation based on the navier stokes equation.

## # 2021-05-13
- Created project for project.
- Decided to use SFML as my graphics library.
- Started working on the first version of the grid that will display the fluid simulation. I call this class MeshImage.

### Why SFML
I decided to use SFML as my graphics library since I have tried SDL2 before and wanted to try this library out as well. I am also comfortable with object oriented programming, which SFML is based on unlike SDL2 or OpenGL is. I was also interested in using the OpenGL API to get more comfortable with it. But I would like to use a newer version than the one we have used in the labs, and from past experience trying to use it during my free time I realized it can be a steep learning curve. From reading online SFML was also easy to learn and use, and also supports the use of shaders which is something I potentially want to make use of. This is also one of the reasons I was thinking of using OpenGL, since I know that SDL does not have native support for shaders.

### MeshImage
The MeshImage class takes a width, height, and resolution on both axis. What I plan for it to do is generate a square mesh consisting of quads. The vertex coordinates will go from *0* to *width* on the horizontal axis and *0* to *height* on the vertical axis. The resolution will dictate how many quads should fit on each axis.  

Each vertex in the quad will store a color which will dictate how the final image looks like. My current plan is to render a single MeshImage that will cover the entire screen.