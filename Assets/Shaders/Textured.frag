#version 450 core
layout(location = 0) out vec4 o_Colour;

in VertexOutput
{
	vec3 Normal;
	vec2 TexCoords;
} v_Input;

layout(location = 2) in flat int v_EntityID;

uniform sampler2D u_Texture;

void main()
{
	o_Colour = texture(u_Texture, v_Input.TexCoords);
}