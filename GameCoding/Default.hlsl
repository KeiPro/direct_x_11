struct VS_INPUT // ���������� ��ü�� ���� ����.
{
    float4 position : POSITION;
    //float4 color : COLOR;
    float2 uv : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION; // SV�� �پ�ߵȴ�.
    //float4 color : COLOR;
    float2 uv : TEXCOORD;
};

cbuffer TrasnformData : register(b0) // buffer�� ����
{
    float4 offset;
}

// IA - VS - RS - PS - OM
// Vertex Shaer �ܰ迡�� ������ input�� 
// ���������� ������ ����̴�.
VS_OUTPUT VS(VS_INPUT input) // ���� �������� ����Ǵ� ���ؽ� ���̴� �Լ�.
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

float4 PS(VS_OUTPUT input) : SV_Target // ��� �ȼ��� ������� �ϴ� �ȼ� ���̴� �Լ�.
{
    float4 color = texture1.Sample(sampler0, input.uv);
    
    return color;
}