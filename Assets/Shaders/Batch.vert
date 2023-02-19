#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoords;
layout(location = 3) in vec4 a_Colour;
layout(location = 4) in float a_TexIndex;
layout(location = 5) in float a_TilingFactor;

uniform mat4 u_ViewProjection;

out VertexOutput
{
	vec3 Normal;
	vec2 TexCoords;
	vec4 Colour;
	float TilingFactor;
} v_Output;

layout(location = 4) out flat float v_TexIndex;

void main()
{
	v_Output.Normal = a_Normal;
	v_Output.TexCoords = a_TexCoords;
	v_Output.Colour = a_Colour;
	v_Output.TilingFactor = a_TilingFactor;
	v_TexIndex = a_TexIndex;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}