#version 450 core

layout(location = 0) out vec4 o_Colour;

in VertexOutput
{
	vec3 Normal;
	vec2 TexCoords;
	vec4 Colour;
	float TilingFactor;
} v_Input;

layout(location = 4) in flat float v_TexIndex;
layout(location = 5) in flat int v_EntityID;

uniform sampler2D u_Textures[32];

void main()
{
	vec4 texcolour = v_Input.Colour;

	switch(int(v_TexIndex))
	{
		case 0: texcolour *= texture(u_Textures[0], v_Input.TexCoords * v_Input.TilingFactor); break;
		case 1: texcolour *= texture(u_Textures[1], v_Input.TexCoords * v_Input.TilingFactor); break;
		case 2: texcolour *= texture(u_Textures[2], v_Input.TexCoords * v_Input.TilingFactor); break;
		case 3: texcolour *= texture(u_Textures[3], v_Input.TexCoords * v_Input.TilingFactor); break;
		case 4: texcolour *= texture(u_Textures[4], v_Input.TexCoords * v_Input.TilingFactor); break;
		case 5: texcolour *= texture(u_Textures[5], v_Input.TexCoords * v_Input.TilingFactor); break;
		case 6: texcolour *= texture(u_Textures[6], v_Input.TexCoords * v_Input.TilingFactor); break;
		case 7: texcolour *= texture(u_Textures[7], v_Input.TexCoords * v_Input.TilingFactor); break;
		case 8: texcolour *= texture(u_Textures[8], v_Input.TexCoords * v_Input.TilingFactor); break;
		case 9: texcolour *= texture(u_Textures[9], v_Input.TexCoords * v_Input.TilingFactor); break;
		case 10: texcolour *= texture(u_Textures[10], v_Input.TexCoords * v_Input.TilingFactor); break;
		case 11: texcolour *= texture(u_Textures[11], v_Input.TexCoords * v_Input.TilingFactor); break;
		case 12: texcolour *= texture(u_Textures[12], v_Input.TexCoords * v_Input.TilingFactor); break;
		case 13: texcolour *= texture(u_Textures[13], v_Input.TexCoords * v_Input.TilingFactor); break;
		case 14: texcolour *= texture(u_Textures[14], v_Input.TexCoords * v_Input.TilingFactor); break;
		case 15: texcolour *= texture(u_Textures[15], v_Input.TexCoords * v_Input.TilingFactor); break;
		case 16: texcolour *= texture(u_Textures[16], v_Input.TexCoords * v_Input.TilingFactor); break;
		case 17: texcolour *= texture(u_Textures[17], v_Input.TexCoords * v_Input.TilingFactor); break;
		case 18: texcolour *= texture(u_Textures[18], v_Input.TexCoords * v_Input.TilingFactor); break;
		case 19: texcolour *= texture(u_Textures[19], v_Input.TexCoords * v_Input.TilingFactor); break;
		case 20: texcolour *= texture(u_Textures[20], v_Input.TexCoords * v_Input.TilingFactor); break;
		case 21: texcolour *= texture(u_Textures[21], v_Input.TexCoords * v_Input.TilingFactor); break;
		case 22: texcolour *= texture(u_Textures[22], v_Input.TexCoords * v_Input.TilingFactor); break;
		case 23: texcolour *= texture(u_Textures[23], v_Input.TexCoords * v_Input.TilingFactor); break;
		case 24: texcolour *= texture(u_Textures[24], v_Input.TexCoords * v_Input.TilingFactor); break;
		case 25: texcolour *= texture(u_Textures[25], v_Input.TexCoords * v_Input.TilingFactor); break;
		case 26: texcolour *= texture(u_Textures[26], v_Input.TexCoords * v_Input.TilingFactor); break;
		case 27: texcolour *= texture(u_Textures[27], v_Input.TexCoords * v_Input.TilingFactor); break;
		case 28: texcolour *= texture(u_Textures[28], v_Input.TexCoords * v_Input.TilingFactor); break;
		case 29: texcolour *= texture(u_Textures[29], v_Input.TexCoords * v_Input.TilingFactor); break;
		case 30: texcolour *= texture(u_Textures[30], v_Input.TexCoords * v_Input.TilingFactor); break;
		case 31: texcolour *= texture(u_Textures[31], v_Input.TexCoords * v_Input.TilingFactor); break;
	}
	o_Colour = texcolour;
}