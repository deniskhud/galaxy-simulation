// galaxy_sim.slang

struct Particle {
    float4 position;
    float4 color;
    float4 orbit; // x: a, y: e, z: tilt, w: E0
};

struct SimParams {
    float time;            // суммарное время с начала симуляции
    float maxOrbitalSpeed; // vMax — скорость на плато кривой вращения
    float coreRadius;      // rc — радиус "твёрдотельной" зоны в центре
    uint  particleCount;
};

[[vk::binding(0, 0)]]
RWStructuredBuffer<Particle> particles;

[[vk::push_constant]]
ConstantBuffer<SimParams> params;

[shader("compute")]
[numthreads(256, 1, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint idx = dispatchThreadID.x;
    if (idx >= params.particleCount) return;

    Particle p = particles[idx];

    float a    = p.orbit.x;
    float e    = p.orbit.y;
    float tilt = p.orbit.z;
    float E0   = p.orbit.w;

    // угловая скорость по кривой вращения: возле центра почти твёрдое тело,
    // на больших радиусах скорость v выходит на плато (как на графике из теории)
    float omega = params.maxOrbitalSpeed / sqrt(a * a + params.coreRadius * params.coreRadius);
    float E = E0 + omega * params.time;

    float b = a * sqrt(max(1.0 - e * e, 0.0));
    float c = a * e;

    float xLocal = a * cos(E) - c;
    float zLocal = b * sin(E);

    float x = xLocal * cos(tilt) - zLocal * sin(tilt);
    float z = xLocal * sin(tilt) + zLocal * cos(tilt);

    particles[idx].position = float4(x, p.position.y, z, 1.0);
}