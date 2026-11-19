#version 330 core

layout (location = 0) in vec3 aPosition;

uniform mat4 u_mvp;

out vec3 position;

void main()
{
   // par_shapes cube spans [0, 1]; remap to [-1, 1] so it's centered on the camera.
   // The centered position doubles as the cubemap sampling direction.
   position = aPosition * 2.0 - 1.0;

   gl_Position = u_mvp * vec4(position, 1.0);
}
