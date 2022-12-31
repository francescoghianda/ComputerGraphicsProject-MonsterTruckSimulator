#version 450

layout(set = 0, binding = 0, std140) uniform GlobalUniformBufferObject {
	mat4 view;
	mat4 proj;
	vec3 leftHeadLightPos;
	vec3 leftHeadLightDir;
	vec3 rightHeadLightPos;
	vec3 rightHeadLightDir;
	vec3 headLightsColor;
	vec3 leftRearLightPos;
	vec3 rightRearLightPos;
	vec3 rearLightsColor;
	vec3 skyColor;
	/*float rearLightSize;
	float rearLightDecay;
	float headLightSize;
	float headLightDecay;*/
} gubo;

layout(set = 1, binding = 0) uniform UniformBufferObject {
	mat4 model;
} ubo;

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 texCoord;

layout(location = 0) out vec3 fragViewDir;
layout(location = 1) out vec3 fragNorm;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out vec3 fragPos;

void main() {
	gl_Position = gubo.proj * gubo.view * ubo.model * vec4(pos, 1.0);
	fragViewDir  = (gubo.view[3]).xyz - (ubo.model * vec4(pos,  1.0)).xyz;
	fragNorm     = transpose(inverse(mat3(ubo.model))) * norm; // (ubo.model * vec4(norm, 0.0)).xyz;
	//fragPos = (gubo.proj * vec4(pos, 1.0)).xyz;
	fragPos = (ubo.model * vec4(pos, 1.0)).xyz;
	fragTexCoord = texCoord;
}