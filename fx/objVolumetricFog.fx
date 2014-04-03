float sqDiffuseLightStrength_var;
float sqSpecularLightStrength_var;
float sqSpecularPower_var;
float3 sqViewDirection_var;

float4 vecSkill41;

float4x4 matWorldViewProj;

//////////////////////////////////////////////////////////////////////
struct out_vs // Output to the pixelshader fragment
{
	float4 Pos		: POSITION;
	float  Depth	: TEXCOORD0;
};

out_vs vs_shader(
	float4 inPos : POSITION,
	float3 inNormal : NORMAL,
	float2 inTex : TEXCOORD0)
{
	out_vs Out;

	Out.Pos = mul(inPos, matWorldViewProj);
	Out.Depth = Out.Pos.w;
	return Out;
}

float4 ps_shader(out_vs In) : COLOR0
{
	float4 depth;
	depth.xyz = In.Depth * 0.001;
	depth.a = 1;
	return depth;
}


technique frontside
{
	pass one
	{
		ZWriteEnable = false;
		//ZFunc = Always;
		//AlphaBlendEnable = true;
		//BlendOp = Min;
		//SrcBlend = One;
		//DestBlend = One;
		
		VertexShader = compile vs_2_0 vs_shader();
		PixelShader = compile ps_2_0 ps_shader();
	}
}

technique backside
{
	pass one
	{
		ZWriteEnable = false;
		//ZFunc = Always;
		//AlphaBlendEnable = true;
		//BlendOp = Max;
		//SrcBlend = One;
		//DestBlend = One;
		
		VertexShader = compile vs_2_0 vs_shader();
		PixelShader = compile ps_2_0 ps_shader();
	}
}

// fallback if nothing works
technique fallback { pass one { } }
