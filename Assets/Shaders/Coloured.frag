#version 450 core
layout(location = 0) out vec4 o_Colour;

struct VertexOutput
{
	vec3 Normal;
	vec4 Colour;
};

layout(location = 0) in VertexOutput v_Input;

void main()
{
	o_Colour = v_Input.Colour;
}