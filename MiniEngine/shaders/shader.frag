#version 450
#extension GL_ARB_seperate_shader_objects : enable

// location = 0: Write into the first buffer
layout(location = 0) in vec3 f3ColorVertex;
layout(location = 0) out vec4 colorOut;

void main()
{
	colorOut = vec4(f3ColorVertex, 1.0);
}