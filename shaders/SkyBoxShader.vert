#version 450
//#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform SkyboxUniformBufferObject {
	mat4 model;
	mat4 view;
	mat4 proj;
	vec3 skyColor;
	vec4 progress;
} subo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec3 fragPos;

void main()
{
    fragTexCoord = inTexCoord;
    vec4 pos = subo.proj * subo.view * subo.model * vec4(inPosition, 1.0);
    //gl_Position = pos.xyww;
	fragPos = 0.5 * (inPosition + vec3(1.0, 1.0, 1.0));//;
	gl_Position = pos;
}  