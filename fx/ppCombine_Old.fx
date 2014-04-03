float sqDiffuseLightStrength_var;
float sqSpecularLightStrength_var;
float sqSpecularPower_var;
float3 sqViewDirection_var;

Texture mtlSkin1;
Texture mtlSkin2;
Texture mtlSkin3;
Texture mtlSkin4;

float4 vecSkill1;
float4 vecSkill5;

sampler sTexture = sampler_state { Texture = <mtlSkin1>; MipFilter = Linear; };
sampler sLight = sampler_state { Texture = <mtlSkin2>; MipFilter = Linear; };
sampler sFog = sampler_state { Texture = <mtlSkin3>; MipFilter = Linear; };
sampler sTranslucent = sampler_state { Texture = <mtlSkin4>; MipFilter = Linear; };

float4 pp_shader( float2 Tex : TEXCOORD0 ) : COLOR0 
{
	Tex *= vecSkill1.x;
	float4 Color = tex2D(sTexture, Tex);
	float4 Light = tex2D(sLight, Tex);
	float Fog = tex2D(sFog, Tex);
	
	float3 ResultSolid = Fog * vecSkill5.rgb + (1 - Fog) * Color * Light;
	float4 ResultTransparent = tex2D(sTranslucent, Tex);
	
	float3 Result = ResultTransparent.rgb * ResultTransparent.a + (1 - ResultTransparent.a) * ResultSolid.rgb;
	
	return float4(Result, 1);
}

technique 
{ 
	pass 
	{
		PixelShader = compile ps_2_0 pp_shader();
	}
}