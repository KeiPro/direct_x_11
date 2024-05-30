struct VS_INPUT // 기하학적인 물체의 고유 도형.
{
    float4 position : POSITION;
    //float4 color : COLOR;
    float2 uv : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION; // SV가 붙어야된다.
    //float4 color : COLOR;
    float2 uv : TEXCOORD;
};

cbuffer TrasnformData : register(b0) // buffer의 약자
{
    float4 offset;
}

// IA - VS - RS - PS - OM
// Vertex Shaer 단계에서 들어오는 input은 
// 기하학적인 도형의 모습이다.
VS_OUTPUT VS(VS_INPUT input) // 정점 단위에서 실행되는 버텍스 쉐이더 함수.
{
    VS_OUTPUT output;
    output.position = input.position + offset;
    //output.color = input.color;
    output.uv = input.uv;
    
    return output;
}

Texture2D texture0 : register(t0);
Texture2D texture1 : register(t1);
SamplerState sampler0 : register(s0);

float4 PS(VS_OUTPUT input) : SV_Target // 모든 픽셀을 대상으로 하는 픽셀 쉐이더 함수.
{
    float4 color = texture1.Sample(sampler0, input.uv);
    
    return color;
}