# C-Programming-and-Design

This project creates a small biosphere of the desert floor containing a dragonfly that is sitting on a
stick. Upon a keypress, the dragonfly should take to the air and hover/fly, and then land back on
the stick.

 Hemisphere
The hemisphere is transparent from the outside, i.e. circumnavigating the hemisphere will
provide a view of the internal world from afar. If the observer is inside the hemisphere, the
outside environment maybe simply black or represented by an appropriate environment
map.

At the bottom of the hemisphere is a dusty desert floor, upon which sits a small piece of
wood (i.e. a stick). Both the desert and stick should be fully textured.

 Insect
A 3D model of a dragonfly (or similar), is located in the hemisphere, sitting on the stick.
The dragonfly should be fully textured.

 Animation
The dragonfly’s legs and wings are to be animated. Upon a keypress the dragonfly should
animate its wings and take off to hover or fly around the sphere, returning to land on the
stick. The leg positions should be animated to slowly transition from sitting to flight and
vice versa. Very simple collision detection is required to ensure that the dragonfly remains
within the hemisphere and does not collide with either the stick or the floor.

 Dust
As the dragonfly moves its wings, they disturb the dust on the desert floor, and the air is
filled with a small cloud of dust. This is to be implemented with a particle system

 Lighting
Required lighting includes a global ambient light and (at least) three spot lights lighting the
content of the sphere

 Shadows
Static and non-static objects should cast as well as receive shadows, which implies simple
shadow mapping.

 Cameras

◦ Camera1:
Overview camera on the outside of the sphere initially looking at its center with the
sphere (nearly) filling the viewport

◦ Camera2:
Interior camera with initial position inside the sphere, looking at the dragonfly

◦ Camera3:
Interior camera that tracks the dragonfly as it flies

◦ Camera4:
Close up camera of the dragonfly’s eye

 Controls

◦ ‘ESC’ exits the application
◦ ‘r’ resets the application to its initial state
◦ Cameras are controlled by the cursor keys:
▪ ‘left’/‘right’/’up’/’down’ rotate left/right/up/down, respectively
▪ CTRL + ‘left’/‘right’/’up’/’down’/’page up’/’page down’ panning to
left/right/forward/backward/up/down, respectively
▪ Alternatively, w-a-s-d or i-j-k-l keys may be used instead of the “cursor” keys
◦ Function keys F1 to F4 will select cameras C1 to C4, respectively
◦ Function key F5 allows switching between render modes: wire frame, diffuse (nontextured),
diffuse (textured), diffuse (textured) with displacement mapping, and toon/cel
shading
◦ Function key F12, starts the dragonfly’s flying animation.
◦ Keys 't'/'T' decrease/increase a factor that globally slows/speeds-up time-dependent
effects

 Configuration File
Scene elements and their animations paths as well as lights and camera configurations
are read from a configuration file.
