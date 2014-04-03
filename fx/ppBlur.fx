Texture TargetMap;

float4 vecViewPort;
float4 vecSkill1;	// X= Strength	Y= Threshold

sampler src = sampler_state { Texture = <TargetMap>; MipFilter = Linear; };

float length(float3 vec)
{
	return sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
}

float4 pp_shader(float2 Tex : TEXCOORD0) : COLOR0
{
	float4 Color = float4(0, 0, 0, 1);
	float num = 0;
	int size = 4;
	for(int x = -size; x <= size; x++)
	{
		for(int y = -size; y <= size; y++)
		{
			float2 blurtex = Tex + float2(x, y) * vecSkill1.x * vecViewPort.zw;
			float fac = size - 0.75 * max(abs(x), abs(y));
			Color += tex2D(src, blurtex) * fac;
			num += fac;
		}
	}
	Color = Color / num;
	
	return Color;
}

technique 
{ 
	pass 
	{
		PixelShader = compile ps_3_0 pp_shader();
	}
}