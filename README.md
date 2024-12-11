## Final Project: Toon Tree

This is our final projct - Toon Tree here!

### Division of Work

1. Our group: Tianxing Ji (tji8), Ikenna Ihenatu (iihenatu) and Qingyuan Lin (qlin20).
2. Our Toon Tree consists of mainly three components, which are Toon/Cel shading, L System and Depth of Field. Meanwhile, during the design check and status update, our TA Steward also recommended us to do more fun things - snow effects, tree grow thickness and forest generation.
3. Therefore, for this project, basically, Ikenna Ihenatu is responsible for Toon Shading, Tianxing Ji is responsible for L System and Qingyuan Lin is responsible for Depth of Field. For other effects like snow effects suggested by Steward, are finished by Tianxing Ji.

### Design Choices

1. Toon Tree:

2. L System:
L System is quite mature and not hard for its generation, where axioms, rules and iteration times are needed to generate a L System string. However, it will be really hard to render this string from 2D space to 3D space:
2.1 firstly, rotation angle in 2D space should be expaned to 3D space, which required the shape to be able to ratate along with x, y and z-axises
    . After that, privous primitives that are all in the object space should have a model matrix to help them know where they should be.

3. Depth of Field:

4. Other effects:
4.1 Snow Effects:

4.2 Day Time Change:

4.3 Texture Mapping:

4.4 Shadow Mapping:

### Collaboration/References

Below are the materials are refered into this project.

1. Toon Tree:
1.1.

2. L System:
2.1.

3. Depth of Field:
3.1. 

4. Other effects:
4.1. 

### Known Bugs

1. When you keep pressing the key that moves the camera while change the parameters on the left side, then the key will be activated all the time that keeps moving the camera.
