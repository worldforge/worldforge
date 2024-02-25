#version 330

// Diffuse texture
uniform sampler2D diffuseMap;
uniform vec4 colour;
uniform float alpha_value;

in vec4 oUv0;

out vec4 fragColour;

void main()
{
    vec4 diffuseColour = texture(diffuseMap, oUv0.st);

    fragColour = colour;
    if (diffuseColour.w == 0) {
        fragColour.w = 0;
    } else {
        fragColour.w = alpha_value;
    }

}