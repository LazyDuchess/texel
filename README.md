# Texel Game Engine
Texel is a heavily Work-In-Progress game engine built in C++. Currently it can only compile Wii binaries.

# Features

* Texel uses [GLM](https://github.com/g-truc/glm) as its mathematics library, matrices are then converted to GX (Wii's renderer) via the rendering layer abstraction at render time. This makes it easier to add cross-platform support to the engine.
* Entity and Component systems.
* Scene and camera systems.

Currently Texel builds a test project that allows you to look around a 3D environment. Wii remote pad up/down keys allow you to move forwards and backwards, pad left/right keys allow you to look right and left, holding A and pressing the pad keys allows you to look around. Plus and minus keys adjust the camera FOV and the B key changes the transform animation on the character model.
