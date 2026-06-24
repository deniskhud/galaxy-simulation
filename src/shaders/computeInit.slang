struct Particle {
    float4 position;
    float4 color;
    float4 orbit;
};

struct initParams {
    uint particleCount;
    uint seed;
    float galaxyRadius;
    float diskThickness;
    float maxEccentricity;
    float armCount;
    float armTwist;
    float pad;
};

[[vk::binding(0, 0)]]
RWStructuredBuffer<Particle> particles;

[[vk::push_constant]]
ConstantBuffer<initParams> params;

uint pcg(uint v) {
    uint state = v * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

// возвращает float в [0, 1)
float rand(uint seed, uint idx, uint lane) {
    return float(pcg(pcg(idx) ^ pcg(seed + lane * 1234567u))) / 4294967296.0;
}

// blackbody color (упрощённая версия из твоего CPU кода)
float3 blackbodyColor(float kelvin) {
    float temp = kelvin / 100.0;
    float r, g, b;

    if (temp <= 66.0) r = 1.0;
    else r = clamp(329.698727446 * pow(temp - 60.0, -0.1332047592) / 255.0, 0.0, 1.0);

    if (temp <= 66.0) g = clamp((99.4708025861 * log(temp) - 161.1195681661) / 255.0, 0.0, 1.0);
    else g = clamp(288.1221695283 * pow(temp - 60.0, -0.0755148492) / 255.0, 0.0, 1.0);

    if (temp >= 66.0) b = 1.0;
    else if (temp <= 19.0) b = 0.0;
    else b = clamp((138.5177312231 * log(temp - 10.0) - 305.0447927307) / 255.0, 0.0, 1.0);

    return float3(r, g, b);
}

static const float kPi = 3.14159265358979;

[shader("compute")]
[numthreads(256, 1, 1)]
void main(uint3 dispatchThreadID: SV_DispatchThreadID)
{
    uint idx = dispatchThreadID.x;
    if (idx >= params.particleCount) return;

    // --- воспроизводимые "случайные" числа для каждой частицы ---
    float spread = (rand(params.seed, idx, 0) * 2.0 - 1.0) * 0.15; // [-0.15, 0.15]
    float t = rand(params.seed, idx, 1);                           // [0, 1)
    float heightR = rand(params.seed, idx, 2) * 2.0 - 1.0;         // [-1, 1]
    float anomaly = rand(params.seed, idx, 3) * 2.0 * kPi;
    float tempK = 3000.0 + rand(params.seed, idx, 4) * 6000.0; // [3000, 9000]

    // --- орбитальные параметры ---
    float a = -log(max(1.0 - t, 1e-6)) * params.galaxyRadius * 0.5;
    float rNorm = clamp(a / params.galaxyRadius, 0.0, 1.0);

    uint armIdx = idx % uint(params.armCount);
    float armOffset = float(armIdx) * (2.0 * kPi / params.armCount);
    float tilt = armOffset + rNorm * params.armTwist + spread;

    float e = params.maxEccentricity * sin(kPi * rNorm);
    float b = a * sqrt(max(1.0 - e * e, 0.0));
    float c = a * e;

    // --- начальная позиция (E = anomaly = E0) ---
    float xLocal = a * cos(anomaly) - c;
    float zLocal = b * sin(anomaly);
    float x = xLocal * cos(tilt) - zLocal * sin(tilt);
    float z = xLocal * sin(tilt) + zLocal * cos(tilt);
    float y = heightR * params.diskThickness * (1.0 - t);

    // --- цвет ---
    float3 rgb = blackbodyColor(tempK);
    float lum = pow(tempK / 9000.0, 4.0);

    // --- запись ---
    particles[idx].position = float4(x, y, z, 1.0);
    particles[idx].orbit = float4(a, e, tilt, anomaly);
    particles[idx].color = float4(rgb, lum);
}