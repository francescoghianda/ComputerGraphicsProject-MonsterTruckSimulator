#version 450

layout(set = 0, binding = 0) uniform HoverlayUniformBufferObject {
	mat4 model;
    mat4 proj;
} hubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec3 fragPos;

void main()
{
    fragTexCoord = inTexCoord;
    vec4 pos = hubo.proj * hubo.model * vec4(inPosition, 1.0);
    //gl_Position = pos.xyww;
	fragPos = inPosition;//0.5 * (inPosition + vec3(1.0, 1.0, 1.0));//;
	gl_Position = pos;
}  