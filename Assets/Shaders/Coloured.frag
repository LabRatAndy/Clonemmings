#version 450 core
layout(location = 0) out vec4 o_Colour;

in VertexOutput
{
	vec3 Normal;
	vec4 Colour;
} v_Input;


void main()
{
	o_Colour = v_Input.Colour;
	//o_Colour = vec4(1.0,0.0,1.0,1.0);
}