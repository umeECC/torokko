struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};
cbuffer SCENE_CONSTANTS : register(b2)
{
    float2 scroll_direction;
    float2 scroll_dummy;
};