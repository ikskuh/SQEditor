Texture TargetMap;
Texture mtlSkin1;

float4 vecViewPort;
float4 vecSkill1;	// X= Contrast	Y= Brightness

sampler src = sampler_state { Texture = <TargetMap>; MipFilter = Linear; };

float4 pp_shader(float2 Tex : TEXCOORD0) : COLOR0
{
	float4 Color = tex2D(src, Tex);
	
	//for(int x = -2; x <= 2; x++)
	//{
	//	for(int y = -2; y <= 2; y++)
	//	{
	//		float2 blurtex = Tex + float2(x, y) * vecViewPort.zw * vecSkill1.z;
	//		Color += tex2D(src, blurtex);
	//	}
	//}
	//Color = Color / 25.0;
	
	//float4 contrast = float4(2, 2, 2, 1) * (1 - vecSkill1.x);
	//Color = saturate((Color - 0.5) * contrast + 0.5);
	
	Color *= vecSkill1.y;
	
	//float4 brightness = 1 - vecSkill1.x * float4(2, 2, 2, 1);
	//Color = saturate(Color + brightness);
	
	return Color;
}

technique 
{ 
	pass 
	{
		PixelShader = compile ps_3_0 pp_shader();
	}
}