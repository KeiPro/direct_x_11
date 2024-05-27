struct VS_INPUT
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION; // SV가 붙어야된다.
    float4 color : COLOR;
};


// IA - VS - RS - PS - OM
VS_OUTPUT VS(VS_INPUT input) // 정점 단위에서 실행되는 버텍스 쉐이더 함수.
{
    VS_OUTPUT output;
    output.position = input.position;
    output.color = input.color;
    
    return output;
}

float4 PS(VS_OUTPUT input) : SV_Target // 모든 픽셀을 대상으로 하는 픽셀 쉐이더 함수.
{
    return input.color;
}