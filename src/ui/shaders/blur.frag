#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D image;
uniform bool horizontal;
uniform float intensity;

void main() {
    float weight[5] = float[](0.227027, 0.316216, 0.070270, 0.043326, 0.009911);
    vec2 tex_offset = intensity / textureSize(image, 0);
    vec3 result = texture(image, TexCoord).rgb * weight[0];

    for (int i = 1; i < 5; ++i) {
        if (horizontal) {
            result += texture(image, TexCoord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(image, TexCoord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        } else {
            result += texture(image, TexCoord + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(image, TexCoord - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }
    FragColor = vec4(result, 1.0);
}
