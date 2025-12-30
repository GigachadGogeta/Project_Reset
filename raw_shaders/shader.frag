#version 450

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 outputFragColorData = vec4(fragColor, 1.0); // may be changed with texture implementation soon
    vec3 outputFragColor = outputFragColorData.rgb; 

    // transparency implementation
    float transparency = outputFragColorData.a;
    if (transparency < .1) {
        discard;
    }

    outColor = vec4(outputFragColor, transparency);
}