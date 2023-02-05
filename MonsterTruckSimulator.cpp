// This has been adapted from the Vulkan tutorial

#include "MonsterTruckSimulator.hpp"
#include "Config.h"

const std::string HUMMER_MODEL_PATH = "models/Hummer.obj";
const std::string HUMMER_TEXTURE_PATH = "textures/HummerDiff.png";

const std::string TERRAIN_MODEL_PATH = "models/Terrain.obj";
const std::string TERRAIN_TEXTURE_PATH = "textures/PaloDuroPark.jpg";

const std::string SKY_BOX_CUBE_MODEL_PATH = "models/SkyBoxCube.obj";
const std::string SKY_BOX_STARS_TEXTURE_PATH = "textures/stars.png";
const std::string SKY_BOX_CLOUDS_TEXTURE_PATH = "textures/clouds.png";

const std::string CIRCLE_MODEL_PATH = "models/circle.obj";
const std::string SPEEDOMETER_TEXTURE_PATH = "textures/speedometer.png";
const std::string WATCH_TEXTURE_PATH = "textures/orologio.png";


const std::string RECTANGLE_MODEL_PATH = "models/rectangle.obj";
const std::string SPEEDOMETER_HAND_TEXTURE_PATH = "textures/speedometer_hand.png";

const std::string WATCH_HAND_MODEL_PATH = "models/watch_hand.obj";
const std::string WATCH_HAND_TEXTURE_PATH = "textures/watch_hand.png";


// The uniform buffer object used in this example

struct GlobalUniformBufferObject {
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
	alignas(16) glm::vec3 leftHeadLightPos;
	alignas(16) glm::vec3 leftHeadLightDir;
	alignas(16) glm::vec3 rightHeadLightPos;
	alignas(16) glm::vec3 rightHeadLightDir;
	alignas(16) glm::vec3 headLightsColor;
	alignas(16) glm::vec3 leftRearLightPos;
	alignas(16) glm::vec3 rightRearLightPos;
	alignas(16) glm::vec3 rearLightsColor;
	alignas(16) glm::vec3 skyColor;
	/*alignas(4) float rearLightSize;
	alignas(4) float rearLightDecay;
	alignas(4) float headLightSize;
	alignas(4) float headLightDecay;*/
};

struct UniformBufferObject {
	alignas(16) glm::mat4 model;
};

struct SkyboxUniformBufferObject {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
	alignas(16) glm::vec4 skyColor;
	alignas(16) glm::vec4 progress;
};

struct HoverlayUniformBufferObject {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 proj;
};


//struct LightsUniformBufferObject {
//	alignas(16) glm::vec3 leftSpotLightPos;
//	alignas(16) glm::vec3 leftSpotLightDir;
//	alignas(16) glm::vec3 rightSpotLightPos;
//	alignas(16) glm::vec3 rightSpotLightDir;
//	alignas(16) glm::vec3 spotLightsColor;
//};

struct TerrainInfo {
	float minX;
	float minY;
	float size;
	glm::vec2 center;
};

enum WheelPosition {
	FRONT_RIGHT,
	FRONT_LEFT,
	REAR_RIGHT,
	REAR_LEFT
};

struct HummerInfo {

	const float length;
	const float width;
	const float scale;
	const float minZ;
	const float maxEngineSpeed;// = 0.03f;
	const float maxSpeed;// = 0.1f;
	const float acceleration;// = 0.0015f;
	const float breakDeceleretion;// = 0.002f;
	const float naturalDeceleretion;// = 0.0003f;
	const float rotSpeed;// = glm::radians(60.0f);
	//const float dxHeadlights = 0.6360 * scale;
	//const float dzHeadlights = 0.8683 * scale;
	//const float elevation = 0.0f;
	const bool independentWheels = false;

	const glm::vec3 leftRearLightPos;// = glm::vec3(0.933 * scale, 2.365 * scale, 1.326 * scale);
	const glm::vec3 rightRearLightPos;// = glm::vec3(-0.973 * scale, 2.365 * scale, 1.326 * scale);

	const glm::vec3 leftHeadLightPos; //= glm::vec3(0.628 * scale, -2.731 * scale, 0.881 * scale);
	const glm::vec3 rightHeadLightPos;// = glm::vec3(-0.657 * scale, -2.731 * scale, 0.881 * scale);

	const glm::vec3 leftRearWheelPos;
	const glm::vec3 rightRearWheelPos;

	const glm::vec3 leftFrontWheelPos;
	const glm::vec3 rightFrontWheelPos;

	std::map<WheelPosition, glm::vec3> wheelPositions;

	glm::vec3 pos;
	float speed = 0.0;

	HummerInfo(float length, float width, float minZ, glm::vec3 startPos, Config config): 
		scale(config.getFloat("scale")),
		length(length * config.getFloat("scale")), 
		width(width * config.getFloat("scale")), 
		minZ(minZ), 
		pos(startPos),
		independentWheels(config.getBool("independent_wheels")),

		rightHeadLightPos(config.getVec3("right_head_light_pos")	* config.getFloat("scale")),
		leftHeadLightPos(config.getVec3("left_head_light_pos")		* config.getFloat("scale")),
		rightRearLightPos(config.getVec3("right_rear_light_pos")	* config.getFloat("scale")),
		leftRearLightPos(config.getVec3("left_rear_light_pos")		* config.getFloat("scale")),

		rightFrontWheelPos(config.getVec3("front_right_wheel_pos")	* config.getFloat("scale")),
		leftFrontWheelPos(config.getVec3("front_left_wheel_pos")	* config.getFloat("scale")),
		rightRearWheelPos(config.getVec3("rear_right_wheel_pos")	* config.getFloat("scale")),
		leftRearWheelPos(config.getVec3("rear_left_wheel_pos")		* config.getFloat("scale")),

		maxEngineSpeed(config.getFloat("max_engine_speed")),
		maxSpeed(config.getFloat("max_speed")),
		acceleration(config.getFloat("acceleration")),
		breakDeceleretion(config.getFloat("break_deceleretion")),
		naturalDeceleretion(config.getFloat("natural_deceleretion")),
		rotSpeed(glm::radians(config.getFloat("rot_speed_degree")))


	{

		wheelPositions[FRONT_RIGHT]	= rightFrontWheelPos;
		wheelPositions[FRONT_LEFT]	= leftFrontWheelPos;
		wheelPositions[REAR_RIGHT]	= rightRearWheelPos;
		wheelPositions[REAR_LEFT]	= leftRearWheelPos;

	}
};

//const float HummerInfo::scale = 0.1f;


// MAIN ! 
class MonsterTruckSimulator : public BaseProject {
protected:
	//Config hummerConfig = Config("HummerIndependentWheelsConfig");
	Config hummerConfig = Config("JeepConfig");
	//Config hummerConfig = Config("HummerConfig");

	// Descriptor Layouts [what will be passed to the shaders]
	DescriptorSetLayout globalDSL;
	DescriptorSetLayout objDSL;
	DescriptorSetLayout skyboxDSL;
	DescriptorSetLayout hoverlayDSL;
	//DescriptorSetLayout skyBoxDSL;

	// Pipelines [Shader couples]
	Pipeline P1;
	Pipeline skyBoxPipeline;
	Pipeline hoverlayPipeline;

	// Models, textures and Descriptors (values assigned to the uniforms)
	Model terrainModel;
	Texture terrainTexture;
	DescriptorSet terrainDS; // objDSL
	TerrainInfo terrainInfo;

	Model hummerModel;
	Texture hummerTexture;
	DescriptorSet hummerDS; // objDSL
	HummerInfo* hummerInfo;

	Model wheelModel;
	Texture wheelTexture;
	DescriptorSet wheelDSs[4];

	Model skyBoxModel;
	Texture skyboxStarsTexture;
	Texture skyboxCloudsTexture;
	DescriptorSet skyBoxDS;

	DescriptorSet globalDS; // globalDSL

	Model circleModel;
	DescriptorSet speedometerDS;
	Texture speedometerTexture;

	DescriptorSet watchDS;
	Texture watchTexture;

	Model watchHandModel;
	DescriptorSet watchHandDS;
	Texture watchHandTexture;

	Model rectangleModel;
	DescriptorSet speedometerHandDS;
	Texture speedometerHandTexture;

	//glm::vec3 hummerPos = glm::vec3(0.0, 0.0, 0.0);
	const glm::vec3 defaultCameraDistance = glm::vec3(0.8f, 0.0f, 0.4f);
	glm::vec3 cameraDistance = defaultCameraDistance;
	

	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 2560;
		windowHeight = 1440;
		windowTitle = "Monster Truck Simulator";
		initialBackgroundColor = { 1.0f, 1.0f, 1.0f, 1.0f };

		// Descriptor pool sizes
		uniformBlocksInPool = 12;
		texturesInPool = 12;
		setsInPool = 12;
	}

	// Here you load and setup all your Vulkan objects
	void localInit() {
		// Descriptor Layouts [what will be passed to the shaders]

		objDSL.init(this, {
			// this array contains the binding:
			// first  element : the binding number
			// second element : the time of element (buffer or texture)
			// third  element : the pipeline stage where it will be used
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
			{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
		});


		skyboxDSL.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
			{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
			{2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
			});

		hoverlayDSL.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
			{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
		});

		/*skyBoxDSL.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
			{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
		});*/

		globalDSL.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
		});

		// Pipelines [Shader couples]
		// The last array, is a vector of pointer to the layouts of the sets that will
		// be used in this pipeline. The first element will be set 0, and so on..
		P1.init(this, "shaders/vert.spv", "shaders/frag.spv", { &globalDSL, &objDSL });
		skyBoxPipeline.init(this, "shaders/SkyBoxVert.spv", "shaders/SkyBoxFrag.spv", { &skyboxDSL });
		hoverlayPipeline.init(this, "shaders/hoverlayVert.spv", "shaders/hoverlayFrag.spv", { &hoverlayDSL });

		circleModel.init(this, CIRCLE_MODEL_PATH);
		speedometerTexture.init(this, SPEEDOMETER_TEXTURE_PATH);

		rectangleModel.init(this, RECTANGLE_MODEL_PATH);
		speedometerHandTexture.init(this, SPEEDOMETER_HAND_TEXTURE_PATH);

		speedometerDS.init(this, &hoverlayDSL, {
			{0, UNIFORM, sizeof(HoverlayUniformBufferObject), nullptr},
			{1, TEXTURE, 0, &speedometerTexture},
		});

		watchTexture.init(this, WATCH_TEXTURE_PATH);

		watchDS.init(this, &hoverlayDSL, {
			{0, UNIFORM, sizeof(HoverlayUniformBufferObject), nullptr},
			{1, TEXTURE, 0, &watchTexture},
			});

		speedometerHandDS.init(this, &hoverlayDSL, {
			{0, UNIFORM, sizeof(HoverlayUniformBufferObject), nullptr},
			{1, TEXTURE, 0, &speedometerHandTexture},
		});

		watchHandModel.init(this, WATCH_HAND_MODEL_PATH);
		watchHandTexture.init(this, WATCH_HAND_TEXTURE_PATH);

		watchHandDS.init(this, &hoverlayDSL, {
			{0, UNIFORM, sizeof(HoverlayUniformBufferObject), nullptr},
			{1, TEXTURE, 0, &watchHandTexture},
			});


		// Models, textures and Descriptors (values assigned to the uniforms)
		//hummerModel.init(this, HUMMER_MODEL_PATH);
		//hummerTexture.init(this, HUMMER_TEXTURE_PATH);
		hummerModel.init(this, hummerConfig.get("model_path"));
		hummerTexture.init(this, hummerConfig.get("texture_path"));

		hummerDS.init(this, &objDSL, {
			// the second parameter, is a pointer to the Uniform Set Layout of this set
			// the last parameter is an array, with one element per binding of the set.
			// first  elmenet : the binding number
			// second element : UNIFORM or TEXTURE (an enum) depending on the type
			// third  element : only for UNIFORMs, the size of the corresponding C++ object
			// fourth element : only for TEXTUREs, the pointer to the corresponding texture object
						{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
						{1, TEXTURE, 0, &hummerTexture},
			});

		std::cout << "Ind. wheels: " << hummerConfig.getBool("independent_wheels") << std::endl;

		if (hummerConfig.getBool("independent_wheels")) {

			wheelModel.init(this, hummerConfig.get("wheel_model_path"));
			wheelTexture.init(this, hummerConfig.get("wheel_texture_path"));

			for (int i = 0; i < 4; i++) {
				wheelDSs[i].init(this, &objDSL, {
							{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
							{1, TEXTURE, 0, &wheelTexture},
					});
			}
			
		}
		

		terrainModel.init(this, TERRAIN_MODEL_PATH);
		terrainTexture.init(this, TERRAIN_TEXTURE_PATH);

		terrainDS.init(this, &objDSL, {
						{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
						{1, TEXTURE, 0, &terrainTexture},
			});


		skyBoxModel.init(this, SKY_BOX_CUBE_MODEL_PATH);
		skyboxStarsTexture.init(this, SKY_BOX_STARS_TEXTURE_PATH);
		skyboxCloudsTexture.init(this, SKY_BOX_CLOUDS_TEXTURE_PATH);

		skyBoxDS.init(this, &skyboxDSL, {
						{0, UNIFORM, sizeof(SkyboxUniformBufferObject), nullptr},
						{1, TEXTURE, 0, &skyboxStarsTexture},
						{2, TEXTURE, 0, &skyboxCloudsTexture},
			});


		globalDS.init(this, &globalDSL, {
						{0, UNIFORM, sizeof(GlobalUniformBufferObject), nullptr}
			});

		initInfo();
	}

	void initInfo() {
		float minX = 100.0;
		float minY = 100.0;

		float maxX = 0.0;
		float maxY = 0.0;

		for (Vertex& v : terrainModel.vertices) {

			if (v.pos.x < minX) minX = v.pos.x;
			if (v.pos.y < minY) minY = v.pos.y;

			if (v.pos.x > maxX) maxX = v.pos.x;
			if (v.pos.y > maxY) maxY = v.pos.y;

		}

		float hummerMinZ = 100.0;
		float hummerMinX = 100.0;
		float hummerMaxX = 0.0;
		float hummerMinY = 100.0;
		float hummerMaxY = 0.0;
		//float hummerMaxZ = 0.0;

		for (Vertex& v : hummerModel.vertices) {

			if (v.pos.z < hummerMinZ) hummerMinZ = v.pos.z;

			if (v.pos.x < hummerMinX) hummerMinX = v.pos.x;
			if (v.pos.x > hummerMaxX) hummerMaxX = v.pos.x;

			if (v.pos.y < hummerMinY) hummerMinY = v.pos.y;
			if (v.pos.y > hummerMaxY) hummerMaxY = v.pos.y;
			//if (v.pos.z > hummerMaxZ) hummerMaxZ = v.pos.y;

		}

		float hummerLength = (hummerMaxY - hummerMinY);
		float hummerWidth = (hummerMaxX - hummerMinX);

		hummerMinZ = glm::abs(hummerMinZ);

		/*std::cout << "minX: " << minX << " - minY: " << minY << std::endl;
		std::cout << "maxX: " << maxX << " - maxY: " << maxY << std::endl;
		std::cout << "Size: " << (maxX - minX) << " x " << (maxY - minY) << std::endl;
		std::cout << "Center: (" << (maxX - minX) / 2 + minX << ", " << (maxY - minY) / 2 + minY << ")" << std::endl;*/

		terrainInfo.size = (maxX - minX);
		terrainInfo.minX = minX;
		terrainInfo.minY = minY;
		terrainInfo.center = glm::vec2((maxX - minX) / 2 + minX, (maxY - minY) / 2 + minY);

		//hummerPos = glm::vec3(terrainInfo.center, 2.0);

		std::cout << "Len: " << hummerLength << std::endl;

		hummerInfo = new HummerInfo(hummerLength, hummerWidth, hummerMinZ, glm::vec3(terrainInfo.center, 2.0), hummerConfig);

		std::cout << "Truck lenght: " << hummerInfo->length << std::endl;
	}

	// Here you destroy all the objects you created!		
	void localCleanup() {
		hummerDS.cleanup();
		hummerTexture.cleanup();
		hummerModel.cleanup();

		if (hummerInfo->independentWheels) {

			for (int i = 0; i < 4; i++) {
				wheelDSs[i].cleanup();
			}

			wheelTexture.cleanup();
			wheelModel.cleanup();
		}

		delete hummerInfo;

		terrainDS.cleanup();
		terrainModel.cleanup();
		terrainTexture.cleanup();
		speedometerTexture.cleanup();
		rectangleModel.cleanup();
		speedometerHandTexture.cleanup();

		watchDS.cleanup();
		watchTexture.cleanup();
		watchHandDS.cleanup();
		watchHandTexture.cleanup();
		watchHandModel.cleanup();
		speedometerDS.cleanup();
		speedometerHandDS.cleanup();
		skyBoxDS.cleanup();
		skyBoxModel.cleanup();
		skyboxStarsTexture.cleanup();
		skyboxCloudsTexture.cleanup();

		circleModel.cleanup();

		globalDS.cleanup();

		P1.cleanup();
		skyBoxPipeline.cleanup();

		hoverlayPipeline.cleanup();

		hoverlayDSL.cleanup();
		skyboxDSL.cleanup();
		globalDSL.cleanup();
		objDSL.cleanup();
	}

	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {

		// SKYBOX PIPELINE

		//GLOBAL

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			skyBoxPipeline.graphicsPipeline);
		/*

		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			skyBoxPipeline.pipelineLayout, 0, 1, &globalDS.descriptorSets[currentImage],
			0, nullptr);*/


		// SKYBOX

		VkBuffer skyBoxVertexBuffers[] = { skyBoxModel.vertexBuffer };
		VkDeviceSize skyBoxOffsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, skyBoxVertexBuffers, skyBoxOffsets);
		vkCmdBindIndexBuffer(commandBuffer, skyBoxModel.indexBuffer, 0,
			VK_INDEX_TYPE_UINT32);

		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			skyBoxPipeline.pipelineLayout, 0, 1, &skyBoxDS.descriptorSets[currentImage],
			0, nullptr);

		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(skyBoxModel.indices.size()), 1, 0, 0, 0);


		// PIPELINE 1

		// GLOBAL

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			P1.graphicsPipeline);

		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			P1.pipelineLayout, 0, 1, &globalDS.descriptorSets[currentImage],
			0, nullptr);



		// HUMMER

		VkBuffer hummerVertexBuffers[] = {hummerModel.vertexBuffer};
		// property .vertexBuffer of models, contains the VkBuffer handle to its vertex buffer
		VkDeviceSize hummerOffsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, hummerVertexBuffers, hummerOffsets);
		// property .indexBuffer of models, contains the VkBuffer handle to its index buffer
		vkCmdBindIndexBuffer(commandBuffer, hummerModel.indexBuffer, 0,
			VK_INDEX_TYPE_UINT32);

		// property .pipelineLayout of a pipeline contains its layout.
		// property .descriptorSets of a descriptor set contains its elements.
		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			P1.pipelineLayout, 1, 1, &hummerDS.descriptorSets[currentImage],
			0, nullptr);

		// property .indices.size() of models, contains the number of triangles * 3 of the mesh.
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(hummerModel.indices.size()), 1, 0, 0, 0);


		//WHEELS

		if (hummerInfo->independentWheels) {

			VkBuffer wheelVertexBuffers[] = { wheelModel.vertexBuffer };
			VkDeviceSize wheelOffsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, wheelVertexBuffers, wheelOffsets);
			vkCmdBindIndexBuffer(commandBuffer, wheelModel.indexBuffer, 0,
				VK_INDEX_TYPE_UINT32);



			for (int i = 0; i < 4; i++) {
				vkCmdBindDescriptorSets(commandBuffer,
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					P1.pipelineLayout, 1, 1, &wheelDSs[i].descriptorSets[currentImage],
					0, nullptr);

				vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(wheelModel.indices.size()), 1, 0, 0, 0);
			}
		}


		// TERRAIN

		VkBuffer terrainVertexBuffers[] = {terrainModel.vertexBuffer};
		VkDeviceSize terrainOffsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, terrainVertexBuffers, terrainOffsets);
		vkCmdBindIndexBuffer(commandBuffer, terrainModel.indexBuffer, 0,
			VK_INDEX_TYPE_UINT32);

		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			P1.pipelineLayout, 1, 1, &terrainDS.descriptorSets[currentImage],
			0, nullptr);

		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(terrainModel.indices.size()), 1, 0, 0, 0);



		// Hoverlay

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			hoverlayPipeline.graphicsPipeline);


		// Speedomenter


		VkBuffer circleModelVertexBuffers[] = { circleModel.vertexBuffer };
		VkDeviceSize circleModelOffsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, circleModelVertexBuffers, circleModelOffsets);
		vkCmdBindIndexBuffer(commandBuffer, circleModel.indexBuffer, 0,
			VK_INDEX_TYPE_UINT32);

		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			hoverlayPipeline.pipelineLayout, 0, 1, &speedometerDS.descriptorSets[currentImage],
			0, nullptr);

		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(circleModel.indices.size()), 1, 0, 0, 0);

		// WATCH

		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			hoverlayPipeline.pipelineLayout, 0, 1, &watchDS.descriptorSets[currentImage],
			0, nullptr);

		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(circleModel.indices.size()), 1, 0, 0, 0);

		// Speedometer hand

		VkBuffer rectangleModelVertexBuffers[] = { rectangleModel.vertexBuffer };
		VkDeviceSize rectangleModelOffsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, rectangleModelVertexBuffers, rectangleModelOffsets);
		vkCmdBindIndexBuffer(commandBuffer, rectangleModel.indexBuffer, 0,
			VK_INDEX_TYPE_UINT32);

		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			hoverlayPipeline.pipelineLayout, 0, 1, &speedometerHandDS.descriptorSets[currentImage],
			0, nullptr);

		//vkCmdPipelineBarrier()

		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(rectangleModel.indices.size()), 1, 0, 0, 0);


		// Watch hand

		VkBuffer watchModelVertexBuffers[] = { watchHandModel.vertexBuffer };
		VkDeviceSize watchModelOffsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, watchModelVertexBuffers, watchModelOffsets);
		vkCmdBindIndexBuffer(commandBuffer, watchHandModel.indexBuffer, 0,
			VK_INDEX_TYPE_UINT32);

		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			hoverlayPipeline.pipelineLayout, 0, 1, &watchHandDS.descriptorSets[currentImage],
			0, nullptr);

		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(watchHandModel.indices.size()), 1, 0, 0, 0);
		
	}

	const bool ALWAYS_DAY = false;

	float getDayTime(float deltaTime, float timeSpeed) {

		if (ALWAYS_DAY) return 12;

		static float dayTime = 0.0;

		float delta = deltaTime * (timeSpeed >= 0 ? timeSpeed : 0.2f);
		dayTime += delta;
		if (dayTime > 24) dayTime = delta;

		return dayTime;
	}

	float getDayTime(float deltaTime) {
		return getDayTime(deltaTime, -1.0);
	}

	struct SkyInfo {
		glm::vec3 skyColor;
		glm::vec4 progress; // ( night, sunrise, day, sunset );
	};

	void getSkyInfo(float time, SkyInfo& skyInfo) {

		static const float sunriseStart = 4;
		static const float sunriseEnd = 9;

		static const float sunsetStart = 18;
		static const float sunsetEnd = 23;

		static const glm::vec3 nightColor = glm::vec3(0.0);
		static const glm::vec3 dayColor = glm::normalize(glm::vec3(135, 206, 250));

		static const glm::vec3 sunsetColor = glm::normalize(glm::vec3(252, 156, 84));
		static const glm::vec3 sunriseColor = glm::normalize(glm::vec3(255, 179, 153));

		glm::vec3 skyColor = glm::vec3(0.0);
		glm::vec4 progressVec = glm::vec4(-1.0);

		if (time > sunsetEnd || time < sunriseStart) {
			//night
			skyColor = nightColor;
			progressVec.x = 1.0;
		}
		else if (time >= sunriseStart && time <= sunriseEnd) {
			//sunrise
			float progress = (time - sunriseStart) / (sunriseEnd - sunriseStart);

			progressVec.y = progress;

			if (progress < 0.5) {
				skyColor = glm::mix(nightColor, sunriseColor, easeInCubic(map(progress, 0, 0.5, 0, 1)));
			}
			else {
				skyColor = glm::mix(sunriseColor, dayColor, easeInCubic(map(progress, 0.5, 1, 0, 1)));
			}

		}
		else if (time > sunriseEnd && time < sunsetStart) {
			//day
			skyColor = dayColor;
			progressVec.z = 1.0;
		}
		else if (time >= sunsetStart && time <= sunsetEnd) {
			//sunset
			float progress = (time - sunsetStart) / (sunsetEnd - sunsetStart);

			progressVec.w = progress;

			if (progress < 0.5) {
				skyColor = glm::mix(dayColor, sunsetColor, easeOutCubic(map(progress, 0, 0.5, 0, 1)));
			}
			else {
				skyColor = glm::mix(sunsetColor, nightColor, easeOutCubic(map(progress, 0.5, 1, 0, 1)));
			}
			
		}

		skyInfo.skyColor = skyColor;
		skyInfo.progress = progressVec;
	}

	float easeInCubic(float n) {
		return n * n * n;
	}

	float easeOutCubic(float n) {
		return 1 - glm::pow(1 - n, 3);
	}

	float map(float number, float currMin, float currMax, float toMin, float toMax) {
		return ((number - currMin) / (currMax - currMin)) * (toMax - toMin) + toMin;
	}

	/*float getPointHeight(glm::vec3& p) {

		float range = 0.00001;

		for (int i = 0; i < terrainModel.indices.size(); i += 3) {
			Vertex v1 = terrainModel.vertices[terrainModel.indices[i]];
			Vertex v2 = terrainModel.vertices[terrainModel.indices[i + 1]];
			Vertex v3 = terrainModel.vertices[terrainModel.indices[i + 2]];

			float areaTot = triangleArea(v1.pos, v2.pos, v3.pos);

			float A1 = triangleArea(p, v2.pos, v3.pos);
			float A2 = triangleArea(v1.pos, p, v3.pos);
			float A3 = triangleArea(v1.pos, v2.pos, p);

			float sum = (A1 + A2 + A3);

			if (sum >= areaTot - range && sum <= areaTot + range) {

				glm::vec3 d = glm::normalize(glm::vec3(pointDistance2D(p, v1.pos), pointDistance2D(p, v2.pos), pointDistance2D(p, v3.pos)));

				return (v1.pos.z * (1 - d.x) + v2.pos.z * (1 - d.y) + v3.pos.z * (1 - d.z)) / ((1 - d.x) + (1 - d.y) + (1 - d.z));

				//float d1 = pointDistance2D(p, v1.pos);
				//float d2 = pointDistance2D(p, v2.pos);
				//float d3 = pointDistance2D(p, v3.pos);

				//float max = glm::max(d1, glm::max(d2, d3));

				//d1 = d1 / max;
				//d2 = d2 / max;
				//d3 = d3 / max;

				//return (v1.pos.z * d1 + v2.pos.z * d2 + v3.pos.z * d3) / (d1 + d2 + d3);
			}
		}

		return 0;
	}*/

	struct HummerSurfaceHeights {
		float center;
		float front;
		float rear;
		float right;
		float left;
	};

	void getHummerSurfaceHeights(glm::vec3& hummerCenter, glm::vec3& hummerFront, glm::vec3& hummerRear, glm::vec3& hummerRight, glm::vec3& hummerLeft, HummerSurfaceHeights& heights) {

		bool centerFound = false, frontFound = false, rearFound = false, rightFound = false, leftFound = false;

		for (int i = 0; i < terrainModel.indices.size(); i += 3) {
			Vertex v1 = terrainModel.vertices[terrainModel.indices[i]];
			Vertex v2 = terrainModel.vertices[terrainModel.indices[i + 1]];
			Vertex v3 = terrainModel.vertices[terrainModel.indices[i + 2]];

			float areaTot = triangleArea(v1.pos, v2.pos, v3.pos);

			if (!centerFound && isPointInTriangle(hummerCenter, v1, v2, v3, areaTot)) {
				glm::vec3 d = glm::normalize(glm::vec3(pointDistance2D(hummerCenter, v1.pos), pointDistance2D(hummerCenter, v2.pos), pointDistance2D(hummerCenter, v3.pos)));
				heights.center = (v1.pos.z * (1 - d.x) + v2.pos.z * (1 - d.y) + v3.pos.z * (1 - d.z)) / ((1 - d.x) + (1 - d.y) + (1 - d.z));
				centerFound = true;
			}

			if (!frontFound && isPointInTriangle(hummerFront, v1, v2, v3, areaTot)) {
				glm::vec3 d = glm::normalize(glm::vec3(pointDistance2D(hummerFront, v1.pos), pointDistance2D(hummerFront, v2.pos), pointDistance2D(hummerFront, v3.pos)));
				heights.front = (v1.pos.z * (1 - d.x) + v2.pos.z * (1 - d.y) + v3.pos.z * (1 - d.z)) / ((1 - d.x) + (1 - d.y) + (1 - d.z));
				frontFound = true;
			}

			if (!rearFound && isPointInTriangle(hummerRear, v1, v2, v3, areaTot)) {
				glm::vec3 d = glm::normalize(glm::vec3(pointDistance2D(hummerRear, v1.pos), pointDistance2D(hummerRear, v2.pos), pointDistance2D(hummerRear, v3.pos)));
				heights.rear = (v1.pos.z * (1 - d.x) + v2.pos.z * (1 - d.y) + v3.pos.z * (1 - d.z)) / ((1 - d.x) + (1 - d.y) + (1 - d.z));
				rearFound = true;
			}

			if (!rightFound && isPointInTriangle(hummerRight, v1, v2, v3, areaTot)) {
				glm::vec3 d = glm::normalize(glm::vec3(pointDistance2D(hummerRight, v1.pos), pointDistance2D(hummerRight, v2.pos), pointDistance2D(hummerRight, v3.pos)));
				heights.right = (v1.pos.z * (1 - d.x) + v2.pos.z * (1 - d.y) + v3.pos.z * (1 - d.z)) / ((1 - d.x) + (1 - d.y) + (1 - d.z));
				rightFound = true;
			}

			if (!leftFound && isPointInTriangle(hummerLeft, v1, v2, v3, areaTot)) {
				glm::vec3 d = glm::normalize(glm::vec3(pointDistance2D(hummerLeft, v1.pos), pointDistance2D(hummerLeft, v2.pos), pointDistance2D(hummerLeft, v3.pos)));
				heights.left = (v1.pos.z * (1 - d.x) + v2.pos.z * (1 - d.y) + v3.pos.z * (1 - d.z)) / ((1 - d.x) + (1 - d.y) + (1 - d.z));
				leftFound = true;
			}

			if (frontFound && rearFound && rightFound && leftFound) return;
		}

	}

	float triangleArea(glm::vec3& v1, glm::vec3& v2, glm::vec3& v3) {
		return 0.5 * glm::abs((v1.x - v3.x) * (v2.y - v1.y) - (v1.x - v2.x) * (v3.y - v1.y));
	}

	bool isPointInTriangle(glm::vec3& p, Vertex& v1, Vertex& v2, Vertex& v3, float area) {

		static float range = 0.00001;

		float A1 = triangleArea(p, v2.pos, v3.pos);
		float A2 = triangleArea(v1.pos, p, v3.pos);
		float A3 = triangleArea(v1.pos, v2.pos, p);

		float sum = (A1 + A2 + A3);

		return sum >= area - range && sum <= area + range;
	}

	float pointDistance2D(glm::vec3& p1, glm::vec3& p2) {
		return glm::sqrt(glm::pow(p1.x - p2.x, 2) + glm::pow(p1.y - p2.y, 2));
	}

	bool isInMap(glm::vec3& hummerFront, glm::vec3& hummerRear, glm::vec3& hummerRight, glm::vec3& hummerLeft) {

		float minX = terrainInfo.minX;
		float maxX = terrainInfo.minX + terrainInfo.size;

		float minY = terrainInfo.minY;
		float maxY = terrainInfo.minY + terrainInfo.size;

		bool xInBounds = hummerFront.x >= minX && hummerFront.x <= maxX
			&& hummerRear.x >= minX && hummerRear.x <= maxX
			&& hummerRight.x >= minX && hummerRight.x <= maxX
			&& hummerLeft.x >= minX && hummerLeft.x <= maxX;

		bool yInBounds = hummerFront.y >= minY && hummerFront.y <= maxY
			&& hummerRear.y >= minY && hummerRear.y <= maxY
			&& hummerRight.y >= minY && hummerRight.y <= maxY
			&& hummerLeft.y >= minY && hummerLeft.y <= maxY;

		return xInBounds && yInBounds;
	}

	bool singleKeyPress(GLFWwindow* window, int key) {
		static std::map<int, int> keysStatus;

		int currentKeyStatus = glfwGetKey(window, key);
		
		if (keysStatus.count(key) > 0) {
			int prevKeyStatus = keysStatus[key];
			keysStatus[key] = currentKeyStatus;

			if (prevKeyStatus == currentKeyStatus)
				return GLFW_RELEASE;
			else
				return currentKeyStatus;
		}
		else {
			keysStatus[key] = currentKeyStatus;
			return currentKeyStatus;
		}
	}

	// Here is where you update the uniforms.
	// Very likely this will be where you will be writing the logic of your application.
	void updateUniformBuffer(uint32_t currentImage) {

		static float lastTime = 0;

		static auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period> (currentTime - startTime).count();

		float deltaT = time - lastTime;
		lastTime = time;


		static float manualCameraYaw = 0;

		static float yaw = 0;
		static float roll = 0;
		static float pitch = 0;

		static float wheelPitch = 0.0;

		static float vSlope = 0;

		static float headlightIntensity = 1.0;
		static bool headlightOn = true;
		static bool lockLightSwitch = false;

		const glm::vec3 oldPos = hummerInfo->pos;

		bool reverseGear = false;
		bool breaking = false;

		int controllerConnected = glfwJoystickPresent(GLFW_JOYSTICK_1);

		int count;
		const unsigned char* controllerBtns = controllerConnected ? glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count) : nullptr;


		/*for (int i = 0; i < count; i++) {
			std::cout << (controllerBtns[i] == GLFW_PRESS ? "1" : "0") << " ";

		}
		std::cout << std::endl;*/
		
		const float* controllerAxes = controllerConnected ? glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count) : nullptr;
		/*
		0 = left stick - left (-1) / right (1)
		1 = left stick - up (-1) / down (1)
		2 = right stick - left (-1) / right (1)
		3 = right stick - up (-1) / down (1)
		4 = LT
		5 = RT
		*/

		/*for (int i = 0; i < count; i++) {
			std::cout << controllerAxes[i] << " ";

		}
		std::cout << std::endl;*/

		// Headlight switch
		
		if (glfwGetKey(window, GLFW_KEY_H) && !lockLightSwitch) {
			headlightOn = !headlightOn;
			lockLightSwitch = true;
		}

		if (glfwGetKey(window, GLFW_KEY_H) == GLFW_RELEASE) {
			lockLightSwitch = false;
		}

		if (headlightOn && headlightIntensity < 1.0) {
			headlightIntensity += 0.1;
		}
		else if (!headlightOn && headlightIntensity > 0.0) {
			headlightIntensity -= 0.1;
			if (headlightIntensity < 0.1) headlightIntensity = 0.0;
		}

		// Go forward
		bool goForward = glfwGetKey(window, GLFW_KEY_W) || (controllerConnected && controllerAxes[5] > -1);

		if (goForward) {
			float enginePower = (glfwGetKey(window, GLFW_KEY_W) ? 1.0 : map(controllerAxes[5], -1.0, 1.0, 0.0, 1.0));
			if (hummerInfo->speed < hummerInfo->maxEngineSpeed * enginePower) hummerInfo->speed += hummerInfo->acceleration * enginePower;
		}

		// Go backward
		bool goBackward = glfwGetKey(window, GLFW_KEY_S) || (controllerConnected && controllerAxes[4] > -1);

		if (goBackward) {
			float enginePower = (glfwGetKey(window, GLFW_KEY_S) ? 1.0 : map(controllerAxes[4], -1.0, 1.0, 0.0, 1.0));
			if (hummerInfo->speed > -hummerInfo->maxEngineSpeed * enginePower) hummerInfo->speed -= hummerInfo->acceleration * enginePower;

			if (hummerInfo->speed < 0) reverseGear = true;
			else breaking = true;
		}


		// Natural acceleration / deceleration
		if (glm::abs(vSlope) >= 0.2f) {

			float acceleration = hummerInfo->naturalDeceleretion * (glm::abs(vSlope) * 10);

			if (vSlope > 0) {
				// direzione discesa (deve andare avanti)
				hummerInfo->speed += acceleration;
				if (hummerInfo->speed > hummerInfo->maxSpeed) hummerInfo->speed = hummerInfo->maxSpeed;
			}
			else {
				// direzione salita (deve tornare indietro)
				hummerInfo->speed -= acceleration;
				if (hummerInfo->speed < -hummerInfo->maxSpeed) hummerInfo->speed = -hummerInfo->maxSpeed;
			}
		}
		else {

			if (hummerInfo->speed > 0) {
				hummerInfo->speed -= hummerInfo->naturalDeceleretion;
				if (hummerInfo->speed < 0) hummerInfo->speed = 0;
			}
			else {
				hummerInfo->speed += hummerInfo->naturalDeceleretion;
				if (hummerInfo->speed > 0) hummerInfo->speed = 0;
			}
		}

		// Break
		if (glfwGetKey(window, GLFW_KEY_B) || (controllerConnected && controllerBtns[GLFW_GAMEPAD_BUTTON_B])) {
			if (hummerInfo->speed > 0) {
				hummerInfo->speed -= hummerInfo->breakDeceleretion;
				if (hummerInfo->speed < 0) hummerInfo->speed = 0;
			}
			else {
				hummerInfo->speed += hummerInfo->breakDeceleretion;
				if (hummerInfo->speed > 0) hummerInfo->speed = 0;
			}
			breaking = true;
		}


		//Rotation
		float axisValue = controllerConnected ? (glm::abs(controllerAxes[0]) < 0.1 ? 0.0 : controllerAxes[0]) : 0.0;

		bool rotateLeft = glfwGetKey(window, GLFW_KEY_A) || axisValue < 0.0;
		bool rotateRight = glfwGetKey(window, GLFW_KEY_D) || axisValue > 0.0;

		float rotationAxis = 0.0f;

		bool rotationWithKeyboard = glfwGetKey(window, GLFW_KEY_A) || glfwGetKey(window, GLFW_KEY_D);

		if (rotateLeft || rotateRight) {
			rotationAxis = (rotateRight ? -1 : 1) * hummerInfo->rotSpeed * (rotationWithKeyboard ? 1.0 : glm::abs(axisValue));
			if (hummerInfo->speed != 0) {
				yaw += deltaT * rotationAxis;
			}
		}
			

		axisValue = controllerConnected ? (glm::abs(controllerAxes[2]) < 0.1 ? 0.0 : controllerAxes[2]) : 0.0;

		bool rotateCameraLeft = glfwGetKey(window, GLFW_KEY_LEFT) || axisValue < 0.0;
		bool rotateCameraRight = glfwGetKey(window, GLFW_KEY_RIGHT) || axisValue > 0.0;

		if (rotateCameraLeft) {
			float rotSpeed = glm::radians(2.0) * (glfwGetKey(window, GLFW_KEY_LEFT) ? 1.0 : glm::abs(axisValue));
			manualCameraYaw -= rotSpeed;
		}
		else if (rotateCameraRight) {
			float rotSpeed = glm::radians(2.0) * (glfwGetKey(window, GLFW_KEY_RIGHT) ? 1.0 : axisValue);
			manualCameraYaw += rotSpeed;
		}

		axisValue = controllerConnected ? (glm::abs(controllerAxes[3]) < 0.1 ? 0.0 : controllerAxes[3]) : 0.0;

		bool rotateCameraUp = glfwGetKey(window, GLFW_KEY_UP) || axisValue > 0.0;
		bool rotateCameraDown = glfwGetKey(window, GLFW_KEY_DOWN) || axisValue < 0.0;

		if (rotateCameraUp) {
			float delta = 0.01 * (glfwGetKey(window, GLFW_KEY_UP) ? 1.0 : axisValue);
			cameraDistance.z -= delta;
			if (cameraDistance.z < 0.2) cameraDistance.z = 0.2;
		}
		else if (rotateCameraDown) {
			float delta = 0.01 * (glfwGetKey(window, GLFW_KEY_DOWN) ? 1.0 : glm::abs(axisValue));
			cameraDistance.z += delta;
			if (cameraDistance.z > 1.5) cameraDistance.z = 1.5;
		}


		if (glfwGetKey(window, GLFW_KEY_P)) {
			cameraDistance.x -= 0.01;
		}
		if (glfwGetKey(window, GLFW_KEY_L)) {
			cameraDistance.x += 0.01;
		}

		// Reset camera position
		if (glfwGetKey(window, GLFW_KEY_R)) {
			manualCameraYaw = 0.0;
			cameraDistance = defaultCameraDistance;
		}


		double speedX = hummerInfo->speed * sin(yaw);
		double speedZ = -hummerInfo->speed * sin(pitch);
		double speedY = -hummerInfo->speed * cos(yaw);

		hummerInfo->pos += glm::vec3(speedX, speedY, speedZ);


		glm::vec3 hummerFront = hummerInfo->pos + glm::vec3((hummerInfo->length / 2) * glm::sin(yaw),  (-hummerInfo->length / 2) * glm::cos(yaw), 0);
		glm::vec3 hummerRear = hummerInfo->pos + glm::vec3((-hummerInfo->length / 2) * glm::sin(yaw), (hummerInfo->length / 2) * glm::cos(yaw), 0);

		glm::vec3 hummerRight = hummerInfo->pos + glm::vec3((-hummerInfo->width / 2) * glm::cos(yaw), (-hummerInfo->width / 2) * glm::sin(yaw), 0);
		glm::vec3 hummerLeft = hummerInfo->pos + glm::vec3((hummerInfo->width / 2) * glm::cos(yaw), (hummerInfo->width / 2) * glm::sin(yaw), 0);

		if (!isInMap(hummerFront, hummerRear, hummerRight, hummerLeft)) {
			hummerInfo->pos = oldPos;

			hummerFront = hummerInfo->pos + glm::vec3((hummerInfo->length / 2) * glm::sin(yaw), (-hummerInfo->length / 2) * glm::cos(yaw), 0);
			hummerRear = hummerInfo->pos + glm::vec3((-hummerInfo->length / 2) * glm::sin(yaw), (hummerInfo->length / 2) * glm::cos(yaw), 0);

			hummerRight = hummerInfo->pos + glm::vec3((-hummerInfo->width / 2) * glm::cos(yaw), (-hummerInfo->width / 2) * glm::sin(yaw), 0);
			hummerLeft = hummerInfo->pos + glm::vec3((hummerInfo->width / 2) * glm::cos(yaw), (hummerInfo->width / 2) * glm::sin(yaw), 0);
		}

		HummerSurfaceHeights heights;

		getHummerSurfaceHeights(hummerInfo->pos, hummerFront, hummerRear, hummerRight, hummerLeft, heights);

		hummerInfo->pos.z = heights.center;

		float deltaZ_FR = glm::abs(heights.front - heights.rear);
		vSlope = glm::atan(deltaZ_FR / hummerInfo->length) * (heights.front > heights.rear ? -1 : 1);

		pitch = vSlope;

		float deltaZ_RL = glm::abs(heights.right - heights.left);
		float hSlope = glm::atan(deltaZ_RL / hummerInfo->width) * (heights.left > heights.right ? -1 : 1);

		roll = hSlope;

		//////// DEBUG ////////

		/*static float debugPitch = 0;
		static float debugRoll = 0;

		if (glfwGetKey(window, GLFW_KEY_O)) {
			debugPitch += glm::radians(2.0);
		}
		if (glfwGetKey(window, GLFW_KEY_K)) {
			debugPitch -= glm::radians(2.0);
		}

		if (glfwGetKey(window, GLFW_KEY_I)) {
			debugRoll += glm::radians(2.0);
		}
		if (glfwGetKey(window, GLFW_KEY_J)) {
			debugRoll -= glm::radians(2.0);
		}

		pitch = debugPitch;
		roll = debugRoll;

		hummerInfo->pos.z = 2;*/

		///////////////////////


		glm::mat3 yawMat = glm::mat3(
			glm::cos(yaw), -glm::sin(yaw), 0,
			glm::sin(yaw), glm::cos(yaw), 0,
			0, 0, 1
		);

		glm::mat3 pitchMat = glm::mat3(
			1, 0, 0,
			0, glm::cos(pitch), -glm::sin(pitch),
			0, glm::sin(pitch), glm::cos(pitch)
		);

		glm::mat3 rollMat = glm::mat3(
			glm::cos(roll), 0, glm::sin(roll),
			0, 1, 0,
			-glm::sin(roll), 0, glm::cos(roll)
		);

		glm::mat3 rotMat = rollMat * pitchMat * yawMat;

		/*std::cout << "Hummer Center: (" << hummerPos.x << ", " << hummerPos.y 
			<< ") - Hummer Front: (" << hummerFront.x << ", " << hummerFront.y << ")"
			<< ") - Hummer Back: (" << hummerBack.x << ", " << hummerBack.y << ")"
			<< std::endl;*/

		/*std::cout << "Hummer Center: (" << hummerPos.x << ", " << hummerPos.y
			<< ") - Hummer Right: (" << hummerRight.x << ", " << hummerRight.y << ")"
			<< ") - Hummer Left: (" << hummerLeft.x << ", " << hummerLeft.y << ")"
			<< std::endl;*/

		
		static bool timeStopped = false;
		float dayTime;

		if (singleKeyPress(window, GLFW_KEY_U)) {
			timeStopped = !timeStopped;
		}

		if (glfwGetKey(window, GLFW_KEY_Y)) dayTime = getDayTime(deltaT, 5.0);
		else if(!timeStopped) dayTime = getDayTime(deltaT);
		else dayTime = getDayTime(deltaT, 0);


		//std::cout << dayTime << std::endl;

		//if (dayTime > 12) dayTime = 24 - dayTime;

		/*glm::vec3 skyColor = glm::vec3(
		0,
		map(dayTime, 0, 24, 0, 0.4),
		map(dayTime, 0, 24, 0, 1)
		);*/

		//std::cout << "Sky color: (" << skyColor.r << ", " << skyColor.g <<
		

		GlobalUniformBufferObject gubo{};
		UniformBufferObject ubo{};
		SkyboxUniformBufferObject subo{};
		//LightsUniformBufferObject lubo{};
		
		void* data;

		float cameraYaw = yaw + glm::radians(90.0) + manualCameraYaw;

		//cameraYaw = yaw;

		glm::vec3 cameraRotation = glm::vec3(
			glm::rotate(glm::mat4(1), cameraYaw, glm::vec3(0, 0, 1)) * 
			glm::vec4(cameraDistance, 1.0f));
		glm::vec3 camPos = hummerInfo->pos + cameraRotation;
		
		gubo.view = glm::lookAt(camPos, hummerInfo->pos, glm::vec3(0.0f, 0.0f, 1.0f));

		gubo.proj = glm::perspective(glm::radians(90.0f),
			swapChainExtent.width / (float)swapChainExtent.height,
			0.02f, 10.0f);
		gubo.proj[1][1] *= -1;

		subo.view = gubo.view;
		subo.proj = glm::perspective(glm::radians(90.0f),
			swapChainExtent.width / (float)swapChainExtent.height,
			0.02f, 40.0f);
		subo.proj[1][1] *= -1;
		// HEADLIGHTS (SPOT LIGHTS)

		gubo.leftHeadLightPos = hummerInfo->pos + hummerInfo->leftHeadLightPos * rotMat;
		gubo.leftHeadLightDir = glm::vec3(glm::sin(yaw), -glm::cos(yaw), -pitch - glm::radians(35.0));

		gubo.rightHeadLightPos = hummerInfo->pos + hummerInfo->rightHeadLightPos * rotMat;
		gubo.rightHeadLightDir = glm::vec3(glm::sin(yaw), -glm::cos(yaw), -pitch - glm::radians(35.0));

		//gubo.headLightSize = 0.225 * hummerInfo->scale;
		//gubo.headLightDecay = 6.0;
		

		gubo.headLightsColor = glm::mix(glm::vec3(0.0), glm::vec3(1.0, 1.0, 0.7), headlightIntensity);


		// REAR LIGHTS
		gubo.rearLightsColor = (reverseGear ? glm::vec3(0.2) : glm::vec3(breaking ? 0.2 : 0.1, 0.0, 0.0)); // / glm::vec3(5.0);

		gubo.leftRearLightPos = hummerInfo->pos + hummerInfo->leftRearLightPos * rotMat;
		gubo.rightRearLightPos = hummerInfo->pos + hummerInfo->rightRearLightPos * rotMat;

		//gubo.rearLightSize = 0.1 * hummerInfo->scale;
		//gubo.rearLightDecay = 2.0;

		SkyInfo skyInfo{};

		getSkyInfo(dayTime, skyInfo);

		gubo.skyColor = skyInfo.skyColor;

		vkMapMemory(device, globalDS.uniformBuffersMemory[0][currentImage], 0,
			sizeof(gubo), 0, &data);
		memcpy(data, &gubo, sizeof(gubo));
		vkUnmapMemory(device, globalDS.uniformBuffersMemory[0][currentImage]);
		

		// HUMMER
		ubo.model =
			glm::translate(glm::mat4(1.0f), hummerInfo->pos) *
			glm::rotate(glm::mat4(1.0f), yaw, glm::vec3(0.0, 0.0, 1.0)) *
			glm::rotate(glm::mat4(1.0f), pitch, glm::vec3(1.0, 0.0, 0.0)) *
			glm::rotate(glm::mat4(1.0f), roll, glm::vec3(0.0, 1.0, 0.0)) *
			glm::scale(glm::mat4(1.0f), glm::vec3(hummerInfo->scale));

		vkMapMemory(device, hummerDS.uniformBuffersMemory[0][currentImage], 0,
			sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, hummerDS.uniformBuffersMemory[0][currentImage]);


		// WHEELS
		if (hummerInfo->independentWheels)
		{
			auto wheelIterator = hummerInfo->wheelPositions.begin();

			wheelPitch += hummerInfo->speed * 10;

			for (int i = 0; i < 4 && wheelIterator != hummerInfo->wheelPositions.end(); i++, wheelIterator++) {

				glm::vec3 wheelPos = hummerInfo->pos + wheelIterator->second * rotMat;

				float wheelRoll = 0.0;
				if (wheelIterator->first == FRONT_RIGHT || wheelIterator->first == REAR_RIGHT)
					wheelRoll = glm::radians(180.0f);

				float wheelYaw = 0.0;
				if (wheelIterator->first == FRONT_LEFT || wheelIterator->first == FRONT_RIGHT)
					wheelYaw = rotationAxis * 0.5;


				ubo.model =
					glm::translate(glm::mat4(1.0f), wheelPos) *
					glm::rotate(glm::mat4(1.0f), yaw, glm::vec3(0.0, 0.0, 1.0)) *
					glm::rotate(glm::mat4(1.0f), pitch, glm::vec3(1.0, 0.0, 0.0)) *
					glm::rotate(glm::mat4(1.0f), roll, glm::vec3(0.0, 1.0, 0.0)) *
					glm::rotate(glm::mat4(1.0f), wheelYaw, glm::vec3(0.0, 0.0, 1.0)) *
					glm::rotate(glm::mat4(1.0f), wheelPitch, glm::vec3(1.0, 0.0, 0.0)) *
					glm::rotate(glm::mat4(1.0f), wheelRoll, glm::vec3(0.0, 1.0, 0.0)) *
					glm::scale(glm::mat4(1.0f), glm::vec3(hummerInfo->scale));

				vkMapMemory(device, wheelDSs[i].uniformBuffersMemory[0][currentImage], 0,
					sizeof(ubo), 0, &data);
				memcpy(data, &ubo, sizeof(ubo));
				vkUnmapMemory(device, wheelDSs[i].uniformBuffersMemory[0][currentImage]);
			}
		}


		// TERRAIN
		ubo.model = glm::mat4(1.0f);
		
		vkMapMemory(device, terrainDS.uniformBuffersMemory[0][currentImage], 0,
			sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, terrainDS.uniformBuffersMemory[0][currentImage]);

		// SKYBOX

		subo.model =
			glm::translate(glm::mat4(1.0f), glm::vec3(terrainInfo.center, 1.0)) *
			glm::scale(glm::mat4(1.0), glm::vec3(20.0));

		subo.skyColor = glm::vec4(skyInfo.skyColor, 1.0);
		subo.progress = skyInfo.progress;

		vkMapMemory(device, skyBoxDS.uniformBuffersMemory[0][currentImage], 0,
			sizeof(subo), 0, &data);
		memcpy(data, &subo, sizeof(subo));
		vkUnmapMemory(device, skyBoxDS.uniformBuffersMemory[0][currentImage]);


		// Hoverlay

		HoverlayUniformBufferObject hubo {};

		float aspectRatio = (float)swapChainExtent.width / (float)swapChainExtent.height;

		// Speedometer

		glm::vec2 speedometerPos(0.8 * aspectRatio, 0.65);

		hubo.model = 
			glm::translate(glm::mat4(1.0), glm::vec3(speedometerPos, -0.1)) *
			glm::scale(glm::mat4(1.0), glm::vec3(0.3));
		hubo.proj = glm::ortho(-1.0f * aspectRatio, 1.0f * aspectRatio, -1.0f, 1.0f, 0.0f, 1.0f);

		vkMapMemory(device, speedometerDS.uniformBuffersMemory[0][currentImage], 0,
			sizeof(subo), 0, &data);
		memcpy(data, &hubo, sizeof(hubo));
		vkUnmapMemory(device, speedometerDS.uniformBuffersMemory[0][currentImage]);


		// Speedometer hand

		constexpr float minAngle = glm::radians(45.0f);
		constexpr float maxAngle = glm::radians(225.0f);

		float speed = ((int)(hummerInfo->speed * 1000.0f)) / 1000.0f;

		float speedometerAngle = map(glm::abs(speed), 0.0, hummerInfo->maxSpeed, minAngle, maxAngle);

		//std::cout << "Speed: " << hummerInfo->speed << " - Angle: " << speedometerAngle << std::endl;

		hubo.model = 
			glm::translate(glm::mat4(1.0), glm::vec3(speedometerPos, 0.0)) *
			glm::rotate(glm::mat4(1.0), speedometerAngle, glm::vec3(0.0, 0.0, 1.0)) *
			glm::scale(glm::mat4(1.0), glm::vec3(0.04, 0.065, 0.04));
		//hubo.proj = glm::ortho(-1.0f * aspectRatio, 1.0f * aspectRatio, -1.0f, 1.0f);

		vkMapMemory(device, speedometerHandDS.uniformBuffersMemory[0][currentImage], 0,
			sizeof(subo), 0, &data);
		memcpy(data, &hubo, sizeof(hubo));
		vkUnmapMemory(device, speedometerHandDS.uniformBuffersMemory[0][currentImage]);

		// Watch

		glm::vec2 watchPos(0.8 * aspectRatio, -0.65);

		hubo.model =
			glm::translate(glm::mat4(1.0), glm::vec3(watchPos, -0.1)) *
			glm::scale(glm::mat4(1.0), glm::vec3(0.2));
		hubo.proj = glm::ortho(-1.0f * aspectRatio, 1.0f * aspectRatio, -1.0f, 1.0f, 0.0f, 1.0f);

		vkMapMemory(device, watchDS.uniformBuffersMemory[0][currentImage], 0,
			sizeof(subo), 0, &data);
		memcpy(data, &hubo, sizeof(hubo));
		vkUnmapMemory(device, watchDS.uniformBuffersMemory[0][currentImage]);

		// Watch hand

		float watchHandAngle = glm::radians(180.0f + 30.0f * dayTime);

		//std::cout << "Speed: " << hummerInfo->speed << " - Angle: " << speedometerAngle << std::endl;

		hubo.model =
			glm::translate(glm::mat4(1.0), glm::vec3(watchPos, 0.0)) *
			glm::rotate(glm::mat4(1.0), watchHandAngle, glm::vec3(0.0, 0.0, 1.0)) *
			glm::scale(glm::mat4(1.0), glm::vec3(0.04, 0.065, 0.04));
		//hubo.proj = glm::ortho(-1.0f * aspectRatio, 1.0f * aspectRatio, -1.0f, 1.0f);

		vkMapMemory(device, watchHandDS.uniformBuffersMemory[0][currentImage], 0,
			sizeof(subo), 0, &data);
		memcpy(data, &hubo, sizeof(hubo));
		vkUnmapMemory(device, watchHandDS.uniformBuffersMemory[0][currentImage]);
	}
};

// This is the main: probably you do not need to touch this!
int main() {
	MonsterTruckSimulator app;

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}