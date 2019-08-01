cbuffer cb
{
    // matrix coming from constant buffer
    // gpu expect col major
    matrix transf;  
};

float4 main(float3 pos : POSITION) : SV_Position
{
     return mul(float4(pos, 1.0f), transf);
}