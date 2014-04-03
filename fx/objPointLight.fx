float4 vecSkill41;	// Position + Range
float4 vecSkill45;	// Color

float4 vecSkill1;	// X= Normal Strength
					// Y= Specular Strength
					// Z= Specular Power
float4 vecSkill5;	// XYZ= View Direciton
float4x4 matWorldViewProj;

Texture mtlSkin1;
Texture mtlSkin2;
Texture mtlSkin3;

sampler2D sPosition = sampler_state
{
	texture = <mtlSkin1>;
};
sampler2D sNormal = sampler_state
{
	texture = <mtlSkin2>;
};
sampler2D sAttribute = sampler_state	//R= Specularity	G= Reflectivity		B= Self-Illuminance		A= Not Used
{
	texture = <mtlSkin3>;
};

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
	Out.Pos = Out.OutPos = mul(inPos, matWorldViewProj);
	Out.TexPos = inTex;
	return Out;
}

float4 ps_shader(out_vs In) : COLOR
{
	float2 ProjTex;
	ProjTex.xy = In.OutPos.xy / In.OutPos.w;
	ProjTex.xy = ProjTex.xy * 0.5 + 0.5;
	ProjTex.y = 1.0 - ProjTex.y;
	
	float4 Light;
	float3 Position = tex2D(sPosition,ProjTex).xyz;
	float3 Normal = normalize(2 * tex2D(sNormal,ProjTex).xyz - 1);
	float4 Attributes = tex2D(sAttribute,ProjTex);
	
	float3 lightdir = Position - vecSkill41.xyz;	
	float intensity = saturate(-vecSkill1.x * dot(Normal, normalize(lightdir)));

	float3 r = normalize(2 * dot(lightdir, Normal) * Normal - lightdir);
	intensity += Attributes.r * vecSkill1.y * pow(max(dot(r, vecSkill5.xyz), 0), vecSkill1.z);
	
	float3 color = vecSkill45.xyz;
	if(length(color) == 0) color = float3(1, 1, 1);
	
	Light.rgb = saturate(color * (1 - length(lightdir) * vecSkill41.w));
	Light.a = 1;
	
	return Light * intensity;
}


technique
{
	pass one
	{
		ZWriteEnable = false;
		ZFunc = Always;
		AlphaBlendEnable = true;
		SrcBlend = One;
		DestBlend = One;
		CullMode = CW;
		
		VertexShader = compile vs_2_0 vs_shader();
		PixelShader = compile ps_2_0 ps_shader();
	}
}

// fallback if nothing works
technique fallback { pass one { } }
