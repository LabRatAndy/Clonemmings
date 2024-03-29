#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Colour;
layout(location = 2) in int a_EntityID;

uniform mat4 u_ViewProjection;

layout(location = 0) out vec4 v_Colour;
layout(location = 1) out flat int v_EntityID;

void main()
{
    v_Colour = a_Colour;
    v_EntityID = a_EntityID;
    gl_Position = u_ViewProjection * vec4(a_Position,1.0);
}