#version 450 core


layout(location = 0) in vec3 fragColor; 
layout(location = 1) in vec2 texCoord;
layout(location = 0) out vec4 fFragClr;

vec3 uColorC0 = vec3(1.0, 0.0, 1.0);
vec3 uColorC1 = vec3(0.0, 0.68, 0.94);

uniform float uTileSize;

uniform bool uUseInterpolatedColor;
uniform bool procedural;

uniform sampler2D           uTex2d;
uniform bool use_texture;

void main()
{
    vec4 interpolated_color = mix(vec4(1), vec4(fragColor, 1.0), float(uUseInterpolatedColor));
    
    vec2 fragCoord = vec2(gl_FragCoord.xy);

    int tx = int(floor(fragCoord.x / uTileSize));
    int ty = int(floor(fragCoord.y / uTileSize));
    float c = mod(tx + ty, 2);

    vec3 procedural_color;
 
    if(c == 0)
    {
        procedural_color = uColorC0;
    }
    else
    {
        procedural_color = uColorC1;
    }

    vec3 checkerColor = mix(uColorC0, uColorC1, float(c));
  
    vec4 texture_color = vec4(procedural_color,1.0);

    vec3 modulatedColor = uColorC0 * uColorC1;

    vec4 coolr = procedural ? interpolated_color * texture_color : interpolated_color;
    fFragClr = use_texture ? texture(uTex2d, texCoord) * coolr : coolr;

}