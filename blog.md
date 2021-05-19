# Pontus Asp's development blog for DH2323 Project
In this blog I will take notes of my progress and thoughts when working on the DH2323 Project. In this project I am going to program a fluid simulation based on the navier stokes equation.

# # 2021-05-13
- Created project for project.
- Decided to use SFML as my graphics library.
- Started working on the first version of the grid that will display the fluid simulation. I call this class MeshImage.
- Started reading a paper about real-time fluid dynamics for games written by Jos Stam.

## Why SFML
I decided to use SFML as my graphics library since I have tried SDL2 before and wanted to try this library out as well. I am also comfortable with object oriented programming, which SFML is based on unlike SDL2 or OpenGL is. I was also interested in using the OpenGL API to get more comfortable with it. But I would like to use a newer version than the one we have used in the labs, and from past experience trying to use it during my free time I realized it can be a steep learning curve. From reading online SFML was also easy to learn and use, and also supports the use of shaders which is something I potentially want to make use of. This is also one of the reasons I was thinking of using OpenGL, since I know that SDL does not have native support for shaders.

## MeshImage
The MeshImage class takes a width, height, and resolution on both axis. What I plan for it to do is generate a square mesh consisting of quads. The vertex coordinates will go from *0* to *width* on the horizontal axis and *0* to *height* on the vertical axis. The resolution will dictate how many quads should fit on each axis.  

Each vertex in the quad will store a color which will dictate how the final image looks like. My current plan is to render a single MeshImage that will cover the entire screen.

I also ended up adding two coloring functions, one that can set the color of a quad, and one that "paints" color onto vertices instead. This mode also supports painting a color with transparency to blend the original and the new color together, however after implementing it I realized that I will probably end up not using this version.

## Research
I have previosuly searched around for some resources that could be of use in the project, and the first resource that I read and tried to understand properly was.

Real-Time Fluid Dynamics for Games by Jos Stam
URL: https://www.dgp.toronto.edu/public_user/stam/reality/Research/pdf/GDC03.pdf

However, since most of this stuff is new to me I was still a bit confused and also found this paper talked about these concepts on a higher level so it was easier to understand.

Fluid Simulation for Dummies by Mike Ask
URL: https://www.mikeash.com/pyblog/fluid-simulation-for-dummies.html

This paper was based on the previous paper but was extended to 3D (I however will stick to 2D). It was also a bit easier to read and the code was explaied more in detail and had easier to read naming conventions so it was easier to understand the code, so it was a good addition to the other paper.

### Extra: Fun link
While checking out different links on google I found this Fluid Simulation implementation which looks really nice!
https://paveldogreat.github.io/WebGL-Fluid-Simulation/


# # 2021-05-14
- Created FluidSimulation class and started working on implementing the logic for the fluid simulation.

## Working on the implementation
When working on the implementation I went between the two papers that I read yesterday to try to figure out how I should write all my code. After implementing everything needed to diffuse the added density (or well, basically added dye), I noticed that it only diffused on the horizontal plane. So if I would add density at the middle of the screen it would only diffuse towards the sides, not up and down. After I reviewed how the movement of the density was calculated in the Advect function it seems to me this is in fact how the article ment for it to work, so even though I am not completely happy with this I kept going to see how the end result would end up looking.  
After implementing the rest of the code needed for the simulation this issue was still there! So I started suspecting that my implementation was not correct and that the problem I saw after implementing the density stuff *is* actually a real problem. At the point of writing this I still have not solved it but will keep looking for the cause.

### Update on problem
Many hours later I just noticed something that kind of makes me happy, but makes me also feel like I will have an even harder time finding the error. So, my issue is that if I add density it only diffuses to the sides - now I just found out that if there is currently some density added on the rows above or below this row, it does in fact spread! It does not even have to be straight below or above, just as long as the other density is on the neighbouring rows this works, they can even be on the opposite sides of the screen. Now, this makes me wonder how this is possible since the cells does not only have to be neighbouring, so they should basically not interact! (Well, at least not *immediately*). I will keep working on finding the solution now. At least now I know that it is possible for the fluid to spread vertically, kind of.

### Update on solution
Alright so after a lot of debugging (~6 hours!!) without success of finding what could be the problem I took Jos Stam's code and implemented it in another CPP file and added a rendering function that used my Mesh Image class and also a mouse handler. To no surprise it worked, mostly, just fine. All density would suddenly dissappear after less than a second but at least it spread properly.  
So, I started replacing parts of my code with the code from Jos Stam until it would start working to try to see at least which of my functions that didn't work. I managed to replace almost the entirety of my code (or well I did rewrite his to my object oriented approach while doing this). Pretty much all that was left was a macro to convert a 2d coordinate to an 1d array index, which I originally got inspired by from Stam's paper.  
After porting all of Stam's code it ***still*** suffered from the same issue my code did earlier. At this point I had no idea what the issue was, but after a while I noticed that my macro was slightly different from Stam's implementation.

My implementation:
```c++
#define IX(x, y) (x + (N + 2) * y)
```

Stam's implementation:
```c++
#define IX(i,j) ((i)+(N + 2)*(j))
```

If we ignore the different variable names and spacing, we can see that the difference between these two defines is that Stam surrounded his variables with parenthesis. As it turns out, this is important! I have not used macros too much before since C++ is not the language I have most experience with so I was not aware of this.  
This small detail was the cause of my program not working! After reading up on how macros work more closely I also can't understand how my program kind of worked before. At least I learned a good lesson, be careful when working with macros! So in total, this small mistake took me 8 hours to find.


# # 2021-05-15
- Added VectorField class.
- Added simple additive coloring.

## VectorField class
With viewing density working I wanted to make it possible to see our other data as well - velocity. So I felt like a nice way to visualize this would be to create a field of vector arrows that would display the direction and magnitude of the velocity vectors. So I created a class that I called VectorField and another class that I called VectorFieldArrow that will keep the information about each arrow.

At this point I just had to figure out how I would draw the arrows, and since I wanted to keep things lightweight I decided to try to draw the arrows with vertices. I plotted out on a paper how to create the arrow using triangles. SFML have some restrictions on how you can define meshes from verticies, namely if I for instance would want to create a shape out of triangles and one vertex is shared with multiple triangles I will have to redefine this vertex instead of using it multiple times. SFML does however reuse two vertices from the previous triangle, so to make a square still only requires 4 vertices in total, but for a more complex shape you might have to redefine vertices. I created my arrows in a way that did not require extra vertices, but the last face to make up the side of the arrow does overlap with two other triangle faces. But since my arrows will have a solid color I do not think this will be a problem. To display the magnitude of the arrows I simply scaled them up and interpolate the color towards bright yellow (from the standard dark blue).

After I created the shape and was happy with how it looked I just had to add some functions to properly position each arrow. I used some built in functionality of SFML to translate the arrows into place and rotate them, which corresponded pretty closely to how the translations and rotations work in OpenGL.

With all this done, I updated my FluidSimulation class to include the vector field and update each arrow with the corresponding velocity vector. However when I increased the resolution of the simulation (4 pixels per simulated block) I realised that the arrows can't really be drawn at that scale, so I also added a scaling factor in the FluidSimulation class so that if I for instance give it a scale of 0.5 and the simulation is 400 by 400 units big, then the vector field is going to have 200*200 arrows.

## Additive coloring
Because the simulation can fill up with color pretty fast I wanted to give some additional detail when there the density has reached the maximum color at a point (at this point in time I draw the density in red, so we only have 256 levels of detail). I did this by simply clamping the color to go to a maximum of 255, but I also added a small value for green (10 units per 255 of red) and blue (20 units per 255 units of red). So now when the density increases past the limit of red it can still keep going until white, which I also think looks nicer. I am also thinking about being able to add other densities that will have other colors, if I do; I will have to change this coloring function to be additive with the current color of the quad as well, since it right now just overwrites it.


# # 2021-05-16
- Started working on particles but scrapped it.
- Added static objects.

## Particles
While developing this simulation I had in my mind that it would be really nice to have some particles that could fly around using the velocity of the simulation acting as the velocity for the particles. I still want to add this but I realized that to make this effect look better I wanted to be able to have something they could flow around. Before starting this project I wanted to add static objects that the fluid could interact with. So I really wanted to have this in place before I would start implementing particles in the simulation. So I just scrapped the code I had written for it so far.

## Static objects
My first thought about how I would add static objects was that I would make them behave like the walls of the scene - they should counteract all forces applied to them. So what I would then have to do is have some way of keeping track of which cells are going to have this behaviour and which does not. Since the walls work by counteracting forces applied to them they only work in one direction of each axis at a time, so an object that would be a 1 layer thin cell would not work based on this since it can't counteract both sides of it. So I realized the smallest possible object would have to consist of 2x2 cells if I would use this method.

After doing some thinking I implemented two arrays, one for lookup which I call bWall and one to keep track of which cells are "walls", which I call iWall. The bWall array is an array of booleans for each cell, so that checking if a cell is a wall or a normal cell is easy. The other array iWall (or well, std::vector, but I will refer to it as array) is an array of indices (ints) in which I just append indices of the walls that act like cells into. With this implementation it is also easy to see if an index already exists in the iWall since you can now check the value of the corresponding index in bWall.

With this implemented I changed my drawing function to check if the current cell is a wall or not and change what it is rendering depending on this. I also changed my HandleMouse function to add the ability to right click to place the static objects, so now you can just draw out a wall and the fluid will later interact with it. I also added bounds to where you can place the walls since they are placed as a 2x2 object so they do not go outside the simulation when placing at the right or bottom edge.

After this I started working on actually making the walls interact with the simulation, I did this by modifying the function that's adding the surrounding walls already by also going through the iWall array and setting the force for these cells. The implementation for this was pretty similar to how the current walls are implemented. The fact that I also now have a guaranteed neighbouring cell in both the x and y axis makes this even more similar, since those can just be ignored when calculating the forces. So if we are setting the forces in the x direction we will first check if the cell to it's right is a wall, and if it's not then we will check the left. If we find a cell in either of these directions then we will counteract it on that axis. This implementation does have a flaw though. It does not take the velocity of the diagonal cells into account and therefore will not perfectly counteract all forces towards it. However it does do a pretty good job, and the issue is barely noticeable when the static objects are more than 3 cells thick.

However the walls also needs a way to stop fluid from diffusing through them. My implementation on this is also not perfect, but still looks good visually. It also also has the issue of not stopping some of the density from leaking out when the static objects are less than 3 cells thick. I implemented this by making each cell take the average value of it's surrounding cells and dividing by the amount of cells it took the density value from (it does not take other walls into account).

So with this implemented, I now have pretty good static objects that can be placed in the simulation while it is running!


# # 2021-05-17
- Added buttons

## Buttons
After adding the possibility to add in static objects I also felt like it would be nice to erase them somehow. But I felt like it could become complicated to make sure that there will never be too thin static objects left so I instead felt like adding a button to remove all walls would be nice. So I created a button class and made some space at the bottom of the screen and added it there. I also added three other buttons, Pause, Toggle Vector Field and Reset. The reset button is also very handy since you needed to restart the application before if you wanted to restart the scene.

# # 2021-05-19
Put up a showcase video of the project so far on YouTube!  
[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/dIdETOWr6CM/0.jpg)](https://www.youtube.com/watch?v=dIdETOWr6CM)  
Click the image to go to the video.