#version 330 core
// read from vertex buffer
layout (location = 0) in vec2 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 texcoord;

// pass to fragment shader
out vec4 Color;
out vec2 Texcoord;

// uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform float engineTime = 0;
uniform int useWind = 0;

// Wind settings.
uniform float speed = 1.0;
uniform float minStrength = 0.1; // hint_range(0.0, 1.0);
uniform float maxStrength = 0.3; // hint_range(0.0, 1.0);
uniform float strengthScale = 100.0;
uniform float interval = 3.5;
uniform float detail = 1.0;
uniform float distortion = 0; // hint_range(0.0, 1.0);
uniform float heightOffset = 0.0;

float getWind(vec2 vertex, vec2 uv, float timer) {
    vec2 pos = mix(vec2(1.0), vertex, distortion).xy;
    float time = timer * speed + pos.x * pos.y;
    float diff = pow(maxStrength - minStrength, 2.0);
    float strength = clamp(minStrength + diff + sin(time / interval) * diff, minStrength, maxStrength) * strengthScale;
    float wind = (sin(time) + cos(time * detail)) * strength * max(0.0, (1.0-uv.y) - heightOffset);
    return wind;
}

void main()
{
    Color = color;
    Texcoord = texcoord;
    vec2 pos = vec2(position.xy);
    if(useWind == 1) {
        pos.x += getWind(position.xy, texcoord, engineTime);
    }
    gl_Position = proj * view * model * vec4(pos, 0.0, 1.0);
}
