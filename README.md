## Final Project: Toon Tree

This is our final project - Toon Tree here!

### Division of Work

1. Our group: Tianxing Ji (tji8), Ikenna Ihenatu (iihenatu), and Qingyuan Lin (qlin20).
2. Our Toon Tree consists of mainly three components: Toon/Cel shading, L System, and Depth of Field. Meanwhile, during the design check and status update, our TA Steward recommended that we implement more fun things, i.e. snow effects, tree growth thickness, and forest generation.
3. Therefore, for this project, essentially, Ikenna Ihenatu is responsible for Toon Shading and design, Tianxing Ji is responsible for the L-System, and Qingyuan Lin is responsible for Depth of Field. Other effects like snow effects suggested by Steward, are finished by Tianxing Ji.

### Design Choices

1. Toon Shader: Incorporating cel-shading of the tree generated by the L system was not an arduous task, as there were ample resources online that helped in generating the cartoon style. Mainly, it involved augmenting the shader program, particularly the lighting equation by augmenting
   the diffuse factor based on the toon color level and the toon scale factor, which is the reciprocal of the toon color level. The desired effect was produced, although the thin branches of the trees may make it a bit difficult to see the full affect on display.

2. L System:
L System is quite mature and not hard to generate, where axioms, rules, and iteration times are needed to create an L System string. However, it will be tough to render this string from 2D space to 3D space:
2.1. firstly, the rotation angle in 2D space should be expanded to 3D space, which requires the shape to be able to rotate along with x, y, and z-axises.
2.2. After that, privous primitives that are all in the object space should have a model matrix to help them know where they should be. But in this project, to simplify this step, I just generate the base point (the root of the original tree) around the world space (0,0,0). Thus, everything related to model matrix will be easily computed based on the original point along with the nex point computed basd on the length we set for the L System (Scale, Rotate and Translate). Meanwhile, related set up for the 3D space L System interpreter is refered to the below reference.
2.3. Finally, all other steps will the same when we render our previous project's scenefiles.

3. Depth of Field: N/A

4. Other effects:
4.1. Snow Effects: The idea is same when we want to create other primitives. What I did here is to create a array that contains all the particles, which can be rendered by GLPoint easily to create fake snows. The logic will be almost the same but just change the primitive type to GLPoint. And another particle shader is created for this only to make the project clearer.

4.2. Day Time Change: Implementing the day-night cycle involved updating the current color and light direction of the directional light representing the sun based on the input hour from the application window. Each segment of the day (morning, noon, evening, and night) was represented by
color and direction vectors, and based on the hour, the output color and direction would be a mix of the two sections in which the current hour falls into, the same with the light direction.

4.3. Texture Mapping: Texture mapping for this project is used from Action project from Tianxing Ji.

4.4. Shadow Mapping: Shadow mapping entailed rendering the scene from the light's point of view, with everything seen from the light's perspective being lit and everything else being in shadow.

### Collaboration/References

Below are the materials referred to in this project.

1. Toon Shader:
1.1. https://www.youtube.com/watch?v=h15kTY3aWaY (Toon shading tutorial)

2. L System:
2.1. https://morphocode.com/3d-branching-structures-with-rabbit/ (To build for 3D space L System)
2.2. https://people.ece.cornell.edu/land/OldStudentProjects/cs490-94to95/hwchen/#results (Where the L System rules come from)
2.3. Chatgpt is used to help me analyse how to compute the model matrix for diferent shapes generated by L System, which is hard to think without Chatgpt.

3. Depth of Field:
3.1. N/A

4. Other effects:
4.1. Snow Effects: https://www.youtube.com/watch?v=jb7pC_3E8SQ (C++ OpenGL Snow Falling: Explained! in 2D space)
4.2. Day Time Change: None
4.3. Texture Mapping: Directly from our previous project, so none.
4.4. Shadow Mapping: https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping

### Known Bugs

1. When you keep pressing the key that moves the camera while changing the parameters on the left side, then the key will be activated all the time that keeps moving the camera.

### Instruction to Run

1. This project is made based on Qt 6.7.3 for macOS and the laptop is M (ARM64) chip macbook. For other kind of laptops, we have not tested, but Ikenna' windows laptop does not support this project - to see the same thing in the macbook.
