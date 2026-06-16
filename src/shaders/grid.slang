struct VertexIn {
    float2 texPos;
    float3 pos;
};

struct VertexOutput {
    float3 color;
    float4 sv_position : SV_Position;
};

[shader("vertex")]
void VSMain(uint id: SV_VertexID, out float4 outColor: COLOR0) -> float4 {

}

[shader("pixel")]
float PSMain(float4 color: COLOR0) -> SV_Target {
    return color;
}