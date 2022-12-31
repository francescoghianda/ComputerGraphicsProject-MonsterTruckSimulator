#version 450
//#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform SkyboxUniformBufferObject {
	mat4 model;
	mat4 view;
	mat4 proj;
	vec4 skyColor;
	vec4 progress; // ( night, sunrise, day, sunset )
} subo;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 fragPos;

layout(set = 0, binding = 1) uniform sampler2D starsTexture;
layout(set = 0, binding = 2) uniform sampler2D cloudsTexture;


layout(location = 0) out vec4 outColor;

void main() {

	vec4 color = subo.skyColor;

	color += vec4(texture(cloudsTexture, fragTexCoord).rgb, 1.0); // clouds

	if(subo.progress.x >= 0.0){
		//night
		color += vec4(texture(starsTexture, fragTexCoord).rgb, 1.0); // stars
	}
	else if(subo.progress.y >= 0.0){
		//sunrise
		color += vec4(texture(starsTexture, fragTexCoord).rgb, 1.0) * (1.0 - subo.progress.y); // stars fade out
	}
	/*else if(subo.progress.z >= 0.0){
		//day

	}*/
	else if(subo.progress.w >= 0.0){
		//sunset
		color += vec4(texture(starsTexture, fragTexCoord).rgb, 1.0) * subo.progress.w; // stars fade in
	}

	
	outColor = color;

}