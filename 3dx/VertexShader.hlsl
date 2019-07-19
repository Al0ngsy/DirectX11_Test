struct VSOutPut // to return pos & col, sequence is important for PS
{
    float4 pos : SV_POSITION;
    float3 col : COLOR;
};

VSOutPut main(float2 pos : POSITION, float3 col : COLOR)
{
    VSOutPut vsout;
    vsout.pos = float4(pos.x, pos.y, 0.0f, 1.0f);
    vsout.col = col;
    return vsout;
}