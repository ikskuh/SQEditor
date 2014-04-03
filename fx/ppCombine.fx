float4x4 matViewProj;

Texture mtlSkin1;
Texture mtlSkin2;
Texture mtlSkin3;
Texture mtlSkin4;

float4 vecSkill1;
float4 vecSkill5;

sampler sTexture = sampler_state { Texture = <mtlSkin1>; MipFilter = Linear; };
sampler sLight = sampler_state { Texture = <mtlSkin2>; MipFilter = Linear; };
sampler sPosition = sampler_state { Texture = <mtlSkin3>; MipFilter = Linear; };
sampler sTranslucent = sampler_state { Texture = <mtlSkin4>; MipFilter = Linear; };

float4 pp_shader( float2 Tex : TEXCOORD0 ) : COLOR0 
{
	Tex *= vecSkill1.x;
	float4 Color = tex2D(sTexture, Tex);
	float4 Light = tex2D(sLight, Tex);
	float depth = mul(tex2D(sPosition, Tex), matViewProj).w;
	float4 ResultTransparent = tex2D(sTranslucent, Tex);
	
	float Fog = saturate(0.005 * (depth - 300));
	
	float3 ResultColor = ResultTransparent.rgb * ResultTransparent.a + (1 - ResultTransparent.a) * Color.rgb;
	float3 Result = Fog * vecSkill5.rgb + (1 - Fog) * ResultColor * Light;
	
	return float4(Result, 1);
}

technique 
{ 
	pass 
	{
		PixelShader = compile ps_2_0 pp_shader();
	}
}