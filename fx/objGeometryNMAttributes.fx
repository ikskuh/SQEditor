Texture entSkin1;
Texture entSkin2;
Texture entSkin3;

float4 vecAmbient;

float4x4 matWorldViewProj;
float4x4 matWorld;
float3x3 matTangent;

sampler sTexture = sampler_state { Texture = <entSkin1>; MipFilter = Linear; };
sampler sNormalmap = sampler_state { Texture = <entSkin2>; MipFilter = Linear; };
sampler sAttributes = sampler_state { Texture = <entSkin3>; MipFilter = Linear; };

// create a matrix for conversion to tangent space
void CreateTangents(float3 inNormal,float3 inTangent)
{
	matTangent[0] = mul(inTangent,matWorld);
	matTangent[1] = mul(cross(inTangent,inNormal),matWorld);	// binormal
	matTangent[2] = mul(inNormal,matWorld);
}

//////////////////////////////////////////////////////////////////////
struct out_vs // Output to the pixelshader fragment
{
	float4 Pos			: POSITION;
	float2 TexPos		: TEXCOORD0;
	float4 WorldPos		: TEXCOORD1;
	float3x3 MatTangent	: TEXCOORD2;
};

struct out_ps // Output to the pixelshader fragment
{
	float4 Color		: COLOR0;
	float4 Position		: COLOR1;
	float4 Normal		: COLOR2;
	float4 Attributes	: COLOR3;	//R= Specularity	G= Reflectivity		B= Self-Illuminance		A= Not Used
};

out_vs vs_shader(
	float4 inPos : POSITION,
	float3 inNormal : NORMAL,
	float2 inTex : TEXCOORD0,
	float3 inTangent : TEXCOORD2)
{
	out_vs Out;

	Out.Pos = mul(inPos, matWorldViewProj);
	Out.WorldPos = mul(inPos,matWorld);
	Out.TexPos = inTex;
	
	CreateTangents(inNormal, inTangent);
	Out.MatTangent = matTangent;
	
	return Out;
}

out_ps ps_shader(out_vs In)
{
	out_ps Out;
	Out.Color = tex2D(sTexture,In.TexPos);
	clip(Out.Color.a - 0.5);
	Out.Position = In.WorldPos;
	
	float4 normalmap = tex2D(sNormalmap, In.TexPos);
	Out.Normal.xyz = 0.5 + 0.5 * mul(2 * normalmap.xyz - 1, In.MatTangent);	
	Out.Normal.w = 1;
	
	Out.Attributes = tex2D(sAttributes,In.TexPos);
	Out.Attributes.a = 1;
	return Out;
}


technique
{
	pass one
	{
		ZWriteEnable = true;
        AlphaTestEnable = true;
        AlphaBlendEnable = false;
		VertexShader = compile vs_2_0 vs_shader();
		PixelShader = compile ps_2_0 ps_shader();
	}
}

// fallback if nothing works
technique fallback { pass one { } }
