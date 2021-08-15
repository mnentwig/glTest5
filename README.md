# glTest5
Toy project to get (back) in touch with C++ / openGl / Eclipse.
Mainly developed on Linux / Raspberry PI 4 but works also on Windows via mingw 32- and 64 bit).

Based on openGL ES 3.1
Libraries: glew (windows), glfw3, glm

Planned to become greater than Midwinter + Elite (stretch goals: Supreme Commander and Mindustry). If it ever gets finished. Which, at the time of writing, seems extremely unlikely.

Maybe interesting bits so far:
- openGL instanced rendering for recurring structures (model parts), intended for large number of moving objects assembled from templates with few parameters (e.g. line and surface color)
- 1980-style 3D line drawing is actually not at all simple  ("lines" are rendered as true 3D surfaces)
- 

Notes:
Development enviromnet 
Eclipse IDE for Embedded C/C++ Developers (includes Incubating components)
Version: 2021-06 (4.20.0)
Build id: 20210612-2011
Importing the project may require modification to "Window/Project/Properties/C/C++ General/Preprocessor Includes /GNU G++/CDT User setting entries:
- add /usr/include for e.g. GL/xyz e.g. glm and
- add /usr/include/c++/8 for e.g. std::vector 
- needs "contains system headers: off" (?)
Eclipse then needs restart for "Problems" from unresolved references to disappear
