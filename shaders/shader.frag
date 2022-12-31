#version 450

layout(set = 1, binding = 1) uniform sampler2D texSampler;

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

/*layout(set = 0, binding = 2) uniform LightsUniformBufferObject {
	vec3 leftHeadLightPos;
	vec3 leftHeadLightDir;
	vec3 rightHeadLightPos;
	vec3 rightHeadLightDir;
	vec3 headLightsColor;
} lubo;*/

layout(location = 0) in vec3 fragViewDir;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec3 fragPos;

layout(location = 0) out vec4 outColor;

vec3 pointLight(vec3 lightColor, vec3 lightPos, float lightSize, float lightDecay, vec3 fragPos){

	vec3 lx = lightPos - fragPos;
	//vec3 lxn = normalize(lx);

	float g = lightSize;
	float decay = lightDecay;

	return lightColor * pow(g / length(lx), decay);
}

vec3 spotLight(vec3 lightColor, vec3 lightPos, vec3 lightDir, vec3 fragPos) {

	float outerCone = 60.0;
	float innerCone = 0.0;

	float g = 2.0;
	float decay = 0.0;

	vec3 lx = lightPos - fragPos;
	vec3 lxn = normalize(lx);

	float cosAlpha = dot(lxn, lightDir);
	float Cin = cos(radians(innerCone/2));
	float Cout = cos(radians(outerCone/2));
	float dimmingEffect = clamp((cosAlpha - Cout) / (Cin - Cout), 0.0, 1.0);
	
	return lightColor * pow(g / length(lx), decay) * dimmingEffect;
}

vec3 lambertDiffuse(vec3 lx, vec3 lightDir, vec3 norm, vec3 diffColor){
	return  diffColor * clamp(dot(normalize(lx), norm), 0.0, 1.0);
}

vec3 phongSpecular(vec3 lx, vec3 norm, vec3 eyeDir, vec3 specColor, float specPower){
	vec3 lxn = normalize(lx);
	vec3 r = (2.0 * norm * dot(normalize(lxn), norm)) - lxn;
	return specColor * pow(clamp(dot(eyeDir, r), 0.0, 1.0), specPower);
}

vec3 lambertPhongBRDF(vec3 lightPos, vec3 lightDir, vec3 norm, vec3 eyeDir, vec3 fragPos, vec3 diffColor, vec3 specColor, float specPower){
	vec3 lx = lightPos - fragPos;
	return lambertDiffuse(lx, lightDir, norm, diffColor) + phongSpecular(lx, norm, eyeDir, specColor, specPower);
}

vec3 hemisphericAmbient(){
	return vec3(0.0);
}

void main() {
	const vec3  diffColor = texture(texSampler, fragTexCoord).rgb;
	const vec3  specColor = vec3(1.0f, 1.0f, 1.0f);
	//const float specPower = 20.0f;
	//const vec3  L = vec3(0.4, 0.0, 0.3);//vec3(-0.4830f, 0.8365f, -0.2588f);

	
	vec3 norm = normalize(fragNorm);		// Norm
	//vec3 R = -reflect(L, norm);
	vec3 eyeDir = normalize(fragViewDir);	// EyeDir
	
	// Lambert diffuse
	//vec3 diffuse  = diffColor * max(dot(norm,L), 0.0f);
	// Phong specular
	//vec3 specular = specColor * pow(max(dot(R,eyeDir), 0.0f), specPower);
	// Hemispheric ambient
	//vec3 ambient  = (vec3(0.1f,0.1f, 0.1f) * (1.0f + norm.y) + vec3(0.0f,0.0f, 0.1f) * (1.0f - norm.y)) * diffColor;

	vec3 d = vec3(0.0, 0.0, 1.0);
	vec3 skyColor = gubo.skyColor + 0.04;//vec3(0.0, 0.8, 1.0);

	vec3 groundColor = vec3(0.0);
	vec3 ambientSky = ((dot(norm, d) + 1.0) / 2) * skyColor;
	vec3 ambientGround = ((1 - dot(norm, d)) / 2) * groundColor;
	//vec3 eyeDir = -vec3(gubo.view * vec4(0, 0, 0, 1));

	vec3 ambient = (ambientSky + ambientGround) * diffColor;

	// SPOTLIGHTS
	vec3 nLeftSpotLightDir = -normalize(gubo.leftHeadLightDir);
	vec3 nRightSpotLightDir = -normalize(gubo.rightHeadLightDir);

	vec3 leftSpotHeadLight = spotLight(gubo.headLightsColor, gubo.leftHeadLightPos, nLeftSpotLightDir, fragPos) * lambertPhongBRDF(gubo.leftHeadLightPos, gubo.leftHeadLightDir, norm, eyeDir, fragPos, diffColor, specColor, 2.0);
	vec3 rightSpotHeadLight = spotLight(gubo.headLightsColor, gubo.rightHeadLightPos, nRightSpotLightDir, fragPos) * lambertPhongBRDF(gubo.rightHeadLightPos, gubo.rightHeadLightDir, norm, eyeDir, fragPos, diffColor, specColor, 2.0);

	// POINTLIGHTS
	float rearLightSize = 0.01;
	float rearLightDecay = 2.0;

	float headLightSize = 0.013;
	float headLightDecay = 6.0;

	vec3 leftRearLight = pointLight(gubo.rearLightsColor, gubo.leftRearLightPos, rearLightSize, rearLightDecay, fragPos);
	vec3 righRearLight = pointLight(gubo.rearLightsColor, gubo.rightRearLightPos, rearLightSize, rearLightDecay, fragPos);

	vec3 leftPointHeadLight = pointLight(gubo.headLightsColor, gubo.leftHeadLightPos, headLightSize, headLightDecay, fragPos);
	vec3 rightPointHeadLight = pointLight(gubo.headLightsColor, gubo.rightHeadLightPos, headLightSize, headLightDecay, fragPos);
	
	//outColor = vec4(clamp(leftSpotHeadLight + rightSpotHeadLight + leftRearLight + ambient + diffuse + specular, vec3(0.0f), vec3(1.0f)), 1.0f);
	outColor = vec4(clamp(leftSpotHeadLight + rightSpotHeadLight + leftRearLight + righRearLight + leftPointHeadLight + rightPointHeadLight + ambient/6, vec3(0.0f), vec3(1.0f)), texture(texSampler, fragTexCoord).a);

}