#version 450 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoords;

uniform mat4 u_ModelTransform;
uniform mat4 u_ViewProjection;

struct VertexOutput
{
	vec3 Normal;
	vec2 TexCoords;
};

layout(location = 0) out VertexOutput v_Output;

void main()
{
	v_Output.Normal = a_Normal;
	v_Output.TexCoords = a_TexCoords;
	gl_Position = u_ViewProjection * u_ModelTransform * vec4(a_Position, 1.0);
} 