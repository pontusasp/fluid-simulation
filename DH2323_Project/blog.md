# Pontus Asp's blog for DH2323 Project
In this blog I will take notes of my progress and thought when working on the DH2323 Project. In this project I am going to program a fluid simulation based on the navier stokes equation.

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

However, this paper did not go deep at all about how the mathematics works so after finishing it I also read.

Fluid Simulation for Dummies by Mike Ask
URL: https://www.mikeash.com/pyblog/fluid-simulation-for-dummies.html

This paper was based on the previous paper but was extended to 3D (I however will stick to 2D). It was also a bit easier to read and the code was explaied more in detail and had easier to read naming conventions so it was easier to understand the code, so it was a good addition to the other paper.

### Extra: Fun link
While checking out different link on google I found this Fluid Simulation implementation which looks really nice!
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
- Added VectorField class
- Added simple additive coloring