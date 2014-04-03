float sqDiffuseLightStrength_var;
float sqSpecularLightStrength_var;
float sqSpecularPower_var;
float3 sqViewDirection_var;

Texture mtlSkin1;
Texture mtlSkin2;
Texture mtlSkin3;
Texture mtlSkin4;

float4x4 matWorldViewProj;

float4 vecSkill1;	// X= Density
					// Y= Light Influence

sampler sWorld = sampler_state { Texture = <mtlSkin1>; MipFilter = Linear; };
sampler sFogBack = sampler_state { Texture = <mtlSkin2>; MipFilter = Linear; };
sampler sFogFront = sampler_state { Texture = <mtlSkin3>; MipFilter = Linear; };
sampler sLight = sampler_state { Texture = <mtlSkin4>; MipFilter = Linear; };

float4 pp_shader( float2 Tex : TEXCOORD0 ) : COLOR0 
{
	float4 posWorld = tex2D(sWorld,Tex.xy);
	posWorld = mul(posWorld, matWorldViewProj);
	float depthWorld = 0.001 * posWorld.w;
	float4 depthFogBack = tex2D(sFogBack,Tex.xy);
	float4 depthFogFront = tex2D(sFogFront,Tex.xy);
	
	float fog = min(depthFogBack.x, depthWorld) - depthFogFront.x;
	
	float4 light = tex2D(sLight,Tex.xy);
	
	return saturate(vecSkill1.x * fog * (1 + vecSkill1.y * (light.r + light.g + light.b)));
}

technique 
{ 
	pass 
	{
		PixelShader = compile ps_2_0 pp_shader();
	}
}