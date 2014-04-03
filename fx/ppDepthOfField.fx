Texture TargetMap;	// Texture
Texture mtlSkin1;	// Depthmap
Texture mtlSkin2;	// Blurmap

float4 vecViewPort;
float4 vecSkill1;	// X= Depth, Y=Strength (0 - 1), Z=ClearArea

float4x4 matViewProj;

sampler sSource = sampler_state { Texture = <TargetMap>; MipFilter = Linear; };
sampler sPosition = sampler_state { Texture = <mtlSkin1>; MipFilter = Linear; };
sampler sSourceBlur = sampler_state { Texture = <mtlSkin2>; MipFilter = GaussianQuad; };

float4 pp_shader( float2 Tex : TEXCOORD0 ) : COLOR0 
{
	float4 ResultNormal = tex2D(sSource, Tex);
	float4 ResultBlur = tex2D(sSourceBlur, Tex);
	
	float4 Position = tex2D(sPosition, Tex);
	Position = mul(Position, matViewProj);
	
	float blurfac = saturate(vecSkill1.y * 0.01 * max(0, abs(Position.w - vecSkill1.x) - vecSkill1.z));
	
	return ResultBlur * blurfac + (1 - blurfac) * ResultNormal;
}

technique 
{ 
	pass 
	{
		PixelShader = compile ps_2_0 pp_shader();
	}
}