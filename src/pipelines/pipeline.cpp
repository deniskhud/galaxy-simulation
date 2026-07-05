#include "pipeline.hpp"

Pipeline::Pipeline(const VulkanContext& context, const SwapChain& swapChain) : context(context), swapChain(swapChain) {
	createDescriptorSetLayout();
	createPipeline();
	createComputePipeline();
	createInitComputePipeline();
}

bool Pipeline::createPipeline() {
	try {
		auto vertShader = readShaderFile("../src/shaders/vert.spv");
		auto fragShader = readShaderFile("../src/shaders/frag.spv");

		vk::raii::ShaderModule vert = createShaderModule(vertShader);
		vk::raii::ShaderModule frag = createShaderModule(fragShader);

		vk::PipelineShaderStageCreateInfo vertexShaderCreateInfo = {
		    .stage = vk::ShaderStageFlagBits::eVertex, .module = *vert, .pName = "main"
		};

		vk::PipelineShaderStageCreateInfo fragShaderStageInfo{
		    .stage = vk::ShaderStageFlagBits::eFragment, .module = *frag, .pName = "main"
		};
		vk::PipelineShaderStageCreateInfo shaderStages[] = {vertexShaderCreateInfo, fragShaderStageInfo};

		vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {
		    .vertexBindingDescriptionCount = 0,
		    .pVertexBindingDescriptions = nullptr,
		    .vertexAttributeDescriptionCount = 0,
		    .pVertexAttributeDescriptions = nullptr,
		};

		vk::PipelineInputAssemblyStateCreateInfo inputAssembly{
		    .topology = vk::PrimitiveTopology::ePointList,
		    .primitiveRestartEnable = vk::False,
		};

		vk::PipelineViewportStateCreateInfo viewportState{
		    .viewportCount = 1, .pViewports = nullptr, .scissorCount = 1, .pScissors = nullptr
		};

		vk::PipelineRasterizationStateCreateInfo rasterizer{
		    .depthClampEnable = VK_FALSE,
		    .rasterizerDiscardEnable = VK_FALSE,
		    .polygonMode = vk::PolygonMode::eFill,
		    .cullMode = vk::CullModeFlagBits::eNone,
		    .frontFace = vk::FrontFace::eCounterClockwise,
		    .depthBiasEnable = VK_FALSE,
		    .depthBiasConstantFactor = 0.0f,
		    .depthBiasClamp = 0.0f,
		    .depthBiasSlopeFactor = 0.0f,
		    .lineWidth = 1.0f
		};
		vk::PipelineMultisampleStateCreateInfo multisampling{
		    .rasterizationSamples = vk::SampleCountFlagBits::e1,
		    .sampleShadingEnable = VK_FALSE,
		    .minSampleShading = 1.0f,
		    .pSampleMask = nullptr,
		    .alphaToCoverageEnable = VK_FALSE,
		    .alphaToOneEnable = VK_FALSE
		};

		// Create depth stencil state info
		vk::PipelineDepthStencilStateCreateInfo depthStencil{
		    .depthTestEnable = vk::False,
		    .depthWriteEnable = vk::False,
		    .depthCompareOp = vk::CompareOp::eLess,
		    .depthBoundsTestEnable = VK_FALSE,
		    .stencilTestEnable = VK_FALSE,
		    .front = {},
		    .back = {},
		    .minDepthBounds = 0.0f,
		    .maxDepthBounds = 1.0f
		};

		// Create color blend attachment state
		vk::PipelineColorBlendAttachmentState colorBlendAttachment{
		    .blendEnable = vk::True,
		    .srcColorBlendFactor = vk::BlendFactor::eOne,
		    .dstColorBlendFactor = vk::BlendFactor::eOne,
		    .colorBlendOp = vk::BlendOp::eAdd,
		    .srcAlphaBlendFactor = vk::BlendFactor::eOne,
		    .dstAlphaBlendFactor = vk::BlendFactor::eOne,
		    .alphaBlendOp = vk::BlendOp::eAdd,
		    .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
		                      vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
		};

		// Create color blend state info
		std::array blendConstants = {0.0f, 0.0f, 0.0f, 0.0f};
		vk::PipelineColorBlendStateCreateInfo colorBlending{
		    .logicOpEnable = VK_FALSE,
		    .logicOp = vk::LogicOp::eCopy,
		    .attachmentCount = 1,
		    .pAttachments = &colorBlendAttachment,
		    .blendConstants = blendConstants
		};

		// Create dynamic state info
		std::vector dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};

		vk::PipelineDynamicStateCreateInfo dynamicState{
		    .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()), .pDynamicStates = dynamicStates.data()
		};

		// Create pipeline layout
		vk::PipelineLayoutCreateInfo pipelineLayoutInfo{
		    .setLayoutCount = 1,
		    .pSetLayouts = &*descriptorSetLayout,
		    .pushConstantRangeCount = 0,
		    .pPushConstantRanges = nullptr
		};

		pipelineLayout = vk::raii::PipelineLayout(context.getDevice(), pipelineLayoutInfo);
		// Create graphics pipeline
		vk::GraphicsPipelineCreateInfo pipelineInfo{
		    .stageCount = 2,
		    .pStages = shaderStages,
		    .pVertexInputState = &vertexInputStateCreateInfo,
		    .pInputAssemblyState = &inputAssembly,
		    .pViewportState = &viewportState,
		    .pRasterizationState = &rasterizer,
		    .pMultisampleState = &multisampling,
		    .pDepthStencilState = &depthStencil,
		    .pColorBlendState = &colorBlending,
		    .pDynamicState = &dynamicState,
		    .layout = *pipelineLayout,
		    .renderPass = nullptr,
		    .subpass = 0,
		    .basePipelineHandle = nullptr,
		    .basePipelineIndex = -1
		};

		// Create pipeline with dynamic rendering
		vk::Format swapChainFormat = swapChain.getSwapChainImageFormat();
		vk::PipelineRenderingCreateInfo renderingInfo{
		    .colorAttachmentCount = 1,
		    .pColorAttachmentFormats = &swapChainFormat,
		    .depthAttachmentFormat = vk::Format::eUndefined,
		    .stencilAttachmentFormat = vk::Format::eUndefined
		};

		pipelineInfo.pNext = &renderingInfo;

			pipeline = vk::raii::Pipeline(context.getDevice(), nullptr, pipelineInfo);

			LOG_INFO("Pipeline::createPipeline", "Graphics pipeline created");
			return true;
		} catch (const std::exception& e) {
			LOG_ERROR("Pipeline::createPipeline", "Failed to create graphics pipeline: {}", e.what());
			return false;
		}
}

bool Pipeline::createDescriptorSetLayout() {
	try {
		// Create descriptor set layout bindings
		std::array<vk::DescriptorSetLayoutBinding, 2> graphicsBindings = {
		    // camera
		    vk::DescriptorSetLayoutBinding{
		        .binding = 0,
		        .descriptorType = vk::DescriptorType::eUniformBuffer,
		        .descriptorCount = 1,
		        .stageFlags = vk::ShaderStageFlagBits::eVertex,
		    },
		    // particles
		    vk::DescriptorSetLayoutBinding{
		        .binding = 1,
		        .descriptorType = vk::DescriptorType::eStorageBuffer,
		        .descriptorCount = 1,
		        .stageFlags = vk::ShaderStageFlagBits::eVertex,
		    },
		};

		std::array<vk::DescriptorSetLayoutBinding, 1> computeBindings = {
		    vk::DescriptorSetLayoutBinding{
		        .binding = 0,
		        .descriptorType = vk::DescriptorType::eStorageBuffer,
		        .descriptorCount = 1,
		        .stageFlags = vk::ShaderStageFlagBits::eCompute,
		    },
		};

		// Create descriptor set layout
		vk::DescriptorSetLayoutCreateInfo layoutInfo{
		    .bindingCount = static_cast<uint32_t>(graphicsBindings.size()), .pBindings = graphicsBindings.data()
		};
		descriptorSetLayout = vk::raii::DescriptorSetLayout(context.getDevice(), layoutInfo);

		// for compute
		vk::DescriptorSetLayoutCreateInfo computeLayoutInfo{
		    .bindingCount = static_cast<uint32_t>(computeBindings.size()), .pBindings = computeBindings.data()
			};
			computeDescriptorSetLayout = vk::raii::DescriptorSetLayout(context.getDevice(), computeLayoutInfo);
			LOG_INFO("Pipeline::createDescriptorSetLayout", "Descriptor set layouts created");
			return true;
		} catch (std::exception& e) {
			LOG_ERROR("Pipeline::createDescriptorSetLayout", "Failed to create descriptor set layout: {}", e.what());
			return false;
		}
}

std::vector<char> Pipeline::readShaderFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		LOG_ERROR("Pipeline::readShaderFile", "Could not open shader file: {}", filename);
		return {};
	}

	size_t fileSize = file.tellg();
	std::vector<char> resultBuffer(fileSize);

	file.seekg(0);
	file.read(resultBuffer.data(), static_cast<long>(fileSize));
	file.close();

	LOG_INFO("Pipeline::readShaderFile", "Loaded shader file {} ({} bytes)", filename, fileSize);
	return resultBuffer;
}

vk::raii::ShaderModule Pipeline::createShaderModule(const std::vector<char>& code) const {
	vk::ShaderModuleCreateInfo shaderModuleCreateInfo = {
	    .codeSize = code.size(),
	    .pCode = reinterpret_cast<const uint32_t*>(code.data()),
	};
	return {context.getDevice(), shaderModuleCreateInfo};
}

/* compute pipeline */
bool Pipeline::createComputePipeline() {
	try {
		auto compShaderCode = readShaderFile("../src/shaders/comp.spv");
		vk::raii::ShaderModule comp = createShaderModule(compShaderCode);

		vk::PipelineShaderStageCreateInfo compShaderStageInfo{
		    .stage = vk::ShaderStageFlagBits::eCompute, .module = *comp, .pName = "main"
		};

		vk::PushConstantRange pushConstantRange{
		    .stageFlags = vk::ShaderStageFlagBits::eCompute,
		    .offset = 0,
		    .size = sizeof(SimParams),
		};

		vk::PipelineLayoutCreateInfo computeLayoutInfo{
		    .setLayoutCount = 1,
		    .pSetLayouts = &*computeDescriptorSetLayout,
		    .pushConstantRangeCount = 1,
		    .pPushConstantRanges = &pushConstantRange,
		};

		computePipelineLayout = vk::raii::PipelineLayout(context.getDevice(), computeLayoutInfo);

		vk::ComputePipelineCreateInfo computePipelineInfo{
		    .stage = compShaderStageInfo,
		    .layout = *computePipelineLayout,
		};

			computePipeline = vk::raii::Pipeline(context.getDevice(), nullptr, computePipelineInfo);

			LOG_INFO("Pipeline::createComputePipeline", "Compute pipeline created");
			return true;
		} catch (const std::exception& e) {
			LOG_ERROR("Pipeline::createComputePipeline", "Failed to create compute pipeline: {}", e.what());
			return false;
		}
}

bool Pipeline::createInitComputePipeline() {
	try {
		auto shaderCode = readShaderFile("../src/shaders/computeInit.spv");
		auto spv = createShaderModule(shaderCode);

		vk::PipelineShaderStageCreateInfo shaderStageInfo{
		    .stage = vk::ShaderStageFlagBits::eCompute, .module = *spv, .pName = "main"
		};

		vk::PushConstantRange pushConstantRange{
		    .stageFlags = vk::ShaderStageFlagBits::eCompute,
		    .offset = 0,
		    .size = sizeof(InitParams),
		};

		vk::PipelineLayoutCreateInfo layoutInfo{
		    .setLayoutCount = 1,
		    .pSetLayouts = &*computeDescriptorSetLayout,
		    .pushConstantRangeCount = 1,
		    .pPushConstantRanges = &pushConstantRange
		};

		initComputePipelineLayout = vk::raii::PipelineLayout(context.getDevice(), layoutInfo);

		vk::ComputePipelineCreateInfo computePipelineInfo{
		    .stage = shaderStageInfo,
		    .layout = *initComputePipelineLayout,
		};

			initComputePipeline = vk::raii::Pipeline(context.getDevice(), nullptr, computePipelineInfo);

			LOG_INFO("Pipeline::createInitComputePipeline", "Init compute pipeline created");
			return true;
		} catch (const std::exception& e) {
			LOG_ERROR("Pipeline::createInitComputePipeline", "Failed to create init pipeline: {}", e.what());
			return false;
		}
}

/*** getters ***/
const vk::raii::DescriptorSetLayout& Pipeline::getDescriptorSetLayout() const {
	return descriptorSetLayout;
}
const vk::raii::PipelineLayout& Pipeline::getPipelineLayout() const {
	return pipelineLayout;
}
const vk::raii::Pipeline& Pipeline::getPipeline() const {
	return pipeline;
}
const vk::raii::DescriptorSetLayout& Pipeline::getComputeDescriptorSetLayout() const {
	return computeDescriptorSetLayout;
}
const vk::raii::PipelineLayout& Pipeline::getComputePipelineLayout() const {
	return computePipelineLayout;
}
const vk::raii::Pipeline& Pipeline::getComputePipeline() const {
	return computePipeline;
}

const vk::raii::Pipeline& Pipeline::getInitComputePipeline() const {
	return initComputePipeline;
}

const vk::raii::PipelineLayout& Pipeline::getInitPipelineLayout() const {
	return initComputePipelineLayout;
}