#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec4 a_Colour;
layout(location = 3) in int a_EntityID;

uniform mat4 u_ModelTransform;
uniform mat4 u_ViewProjection;

out VertexOutput
{
	vec3 Normal;
	vec4 Colour;
} v_Output;

layout(location = 2) out flat int v_EntityID;

void main()
{
	v_Output.Normal = a_Normal;
	v_Output.Colour = a_Colour;
	v_EntityID = a_EntityID;
	gl_Position = u_ViewProjection * u_ModelTransform * vec4(a_Position,1.0);
}