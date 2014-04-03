Texture entSkin1;
Texture mtlSkin1;

float4 vecAmbient;

float4x4 matWorldViewProj;
float4x4 matViewProj;
float4x4 matWorld;

sampler sTexture = sampler_state { Texture = <entSkin1>; MipFilter = Linear; };
sampler sGeometry = sampler_state { Texture = <mtlSkin1>; MipFilter = Linear; };

//////////////////////////////////////////////////////////////////////
struct out_vs // Output to the pixelshader fragment
{
	float4 Pos		: POSITION;
	float2 TexPos	: TEXCOORD0;
	float4 OutPos	: TEXCOORD1;
};

out_vs vs_shader(
	float4 inPos : POSITION,
	float3 inNormal : NORMAL,
	float2 inTex : TEXCOORD0)
{
	out_vs Out;

	Out.Pos = mul(inPos, matWorldViewProj);
	Out.OutPos = Out.Pos;
	Out.TexPos = inTex;
	return Out;
}

float4 ps_shader(out_vs In) : COLOR
{
	float2 ProjTex;
	ProjTex.xy = In.OutPos.xy / In.OutPos.w;
	ProjTex.xy = ProjTex.xy * 0.5 + 0.5;
	ProjTex.y = 1.0 - ProjTex.y;

	float4 pos = tex2D(sGeometry, ProjTex);
	pos = mul(pos, matViewProj);
	
	clip(pos.w - In.OutPos.w);
	
	float4 Color = tex2D(sTexture,In.TexPos);
	return Color;
}

technique
{
	pass one
	{
		ZWriteEnable = false;
		BlendOp = Add;
		BlendOpAlpha = Max;
		VertexShader = compile vs_2_0 vs_shader();
		PixelShader = compile ps_2_0 ps_shader();
	}
}

// fallback if nothing works
technique fallback { pass one { } }
