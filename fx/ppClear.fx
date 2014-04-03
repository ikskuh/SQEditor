float4 vecSkill1;

Texture mtlSkin1;

sampler srcAttributes = sampler_state { Texture = <mtlSkin1>; MipFilter = Linear; };

struct out_ps
{
	float4 Color0	: COLOR0;	// Transparency map
	float4 Color1	: COLOR1;	// 
	float4 Color2	: COLOR2;	// Light Buffer
};

out_ps pp_shader(float2 Tex : TEXCOORD0)
{
	out_ps Out;
	
	Out.Color0 = float4(0, 0, 0, 0);
	Out.Color1 = float4(0, 0, 0, 1);
	Out.Color2 = vecSkill1.rgba + tex2D(srcAttributes, Tex).g;
	
	return Out;
}

technique 
{ 
	pass 
	{
		PixelShader = compile ps_2_0 pp_shader();
	}
}