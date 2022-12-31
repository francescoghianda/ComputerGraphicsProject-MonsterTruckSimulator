#version 450

layout(set = 0, binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 fragPos;

layout(location = 0) out vec4 outColor;

void main() {

	vec4 color = vec4(texture(texSampler, fragTexCoord).rgb, 1.0);//vec4(1.0, 0.0, 0.0, 1.0);
	
	outColor = color;

}