Texture TargetMap;
Texture mtlSkin1;

sampler src1 = sampler_state { Texture = <TargetMap>; MipFilter = Linear; };
sampler src2 = sampler_state { Texture = <mtlSkin1>; MipFilter = Linear; };

float4 pp_shader(float2 Tex : TEXCOORD0) : COLOR0
{
	return tex2D(src1, Tex) + tex2D(src2, Tex);
}

technique 
{ 
	pass 
	{
		PixelShader = compile ps_3_0 pp_shader();
	}
}