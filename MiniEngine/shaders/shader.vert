#version 450
#extension GL_ARB_seperate_shader_objects : enable

out gl_PerVertex
{
	vec4 gl_Position;
};

layout(location = 0) out vec3 f3ColorVertex;

vec2 positions[3] = vec2[]
(
	vec2( 0.0, -0.5),
	vec2( 0.5,  0.5),
	vec2(-0.5,  0.5)
);

vec3 af3Color[3] = vec3[]
(
	vec3(1.0, 0.0, 0.0),
	vec3(0.0, 1.0, 0.0),
	vec3(1.0, 0.0, 1.0)
);

void main()
{
	gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
	f3ColorVertex = af3Color[gl_VertexIndex];
}