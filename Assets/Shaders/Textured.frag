#version 450 core
layout(location = 0) out vec4 o_Colour;

struct VertexOutput
{
	vec3 Normal;
	vec2 TexCoords;
};

layout(location = 0) in VertexOutput v_Input;

uniform sampler2D u_Texture;

void main()
{
	o_Colour = texture(u_Texture, v_Input.TexCoords);
}