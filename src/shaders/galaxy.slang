struct Particle {
    float4 position;
    float4 color;
};

struct CameraData {
    column_major float4x4 view;
    column_major float4x4 proj;
};

[[vk::binding(0, 0)]]
ConstantBuffer<CameraData> camera;

[[vk::binding(1, 0)]]
StructuredBuffer<Particle> particles;

struct VSOutput {
    float4 position                        : SV_Position;
    [[vk::builtin("PointSize")]] float pointSize : SV_PointSize;
    float4 color                           : Color;
};

[shader("vertex")]
VSOutput VSMain(uint vertexID : SV_VertexID)
{
    VSOutput output;
    Particle p = particles[vertexID];

    float4 worldPos  = float4(p.position.xyz, 1.0);
    output.position  = mul(camera.proj, mul(camera.view, worldPos));
    output.pointSize = 4.0;
    output.color     = p.color;
    return output;
}

struct PSInput {
    float4 color : Color;
};

struct PSOutput {
    float4 color : SV_Target;
};

[shader("fragment")]
PSOutput PSMain(PSInput input)
{
    PSOutput output;
    output.color = input.color;
    return output;
}