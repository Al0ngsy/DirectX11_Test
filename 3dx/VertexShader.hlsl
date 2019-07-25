struct VSOutPut // to return pos & col, sequence is important for PS
{
    float4 pos : SV_POSITION;
    float3 col : COLOR;
};

cbuffer cb
{
    // matrix coming from constant buffer
    // gpu expect col major
    matrix transf;  
};

VSOutPut main(float2 pos : POSITION, float3 col : COLOR)
{
    VSOutPut vsout;
    // multiply given point with the tranformation matrix
    // mul(vector, matrix) built in function
    vsout.pos = mul(float4(pos.x, pos.y, 0.0f, 1.0f), transf);  
    vsout.col = col;
    return vsout;
}