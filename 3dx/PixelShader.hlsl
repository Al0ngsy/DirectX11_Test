float4 main(float4 pos : SV_POSITION, float3 col: COLOR) : SV_TARGET
{
	return float4(col, 1.0f);
}