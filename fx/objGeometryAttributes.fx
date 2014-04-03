Texture entSkin1;
Texture entSkin2;	// Attribute map

float4 vecAmbient;

float4x4 matWorldViewProj;
float4x4 matWorld;

sampler sTexture = sampler_state { Texture = <entSkin1>; MipFilter = Linear; };
sampler sAttributes = sampler_state { Texture = <entSkin2>; MipFilter = Linear; };

//////////////////////////////////////////////////////////////////////
struct out_vs // Output to the pixelshader fragment
{
	float4 Pos		: POSITION;
	float2 TexPos	: TEXCOORD0;
	float4 WorldPos	: TEXCOORD1;
	float3 Normal	: TEXCOORD2;
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
	float2 inTex : TEXCOORD0)
{
	out_vs Out;

	Out.Pos = mul(inPos, matWorldViewProj);
	Out.Normal = mul(inNormal, (float3x3)matWorld);
	Out.WorldPos = mul(inPos,matWorld);
	Out.TexPos = inTex;
	return Out;
}

out_ps ps_shader(out_vs In)
{
	out_ps Out;
	Out.Color = tex2D(sTexture,In.TexPos);
	clip(Out.Color.a - 0.5);
	Out.Position = In.WorldPos;
	Out.Normal = float4(0.5 + 0.5 * In.Normal, 1);
	Out.Attributes = tex2D(sAttributes, In.TexPos);
	
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
