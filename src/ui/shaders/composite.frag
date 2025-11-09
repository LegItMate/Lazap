#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
uniform sampler2D scene;
uniform sampler2D blur;
uniform float opacity; // Optional: control blur strength

void main()
{
    vec4 sceneColor = texture(scene, TexCoords);
    vec4 blurColor = texture(blur, TexCoords);

    // Blend blurred background with the scene based on alpha
    // and avoid writing any solid color like black.
    vec4 result = mix(sceneColor, blurColor, opacity);

    // Keep full transparency where the original scene is transparent
    if (sceneColor.a < 0.01)
    discard;

    FragColor = vec4(result.rgb, sceneColor.a);
}
