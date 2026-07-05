#include "descriptors.hpp"

DescriptorPool::DescriptorPool(const VulkanContext& context, const Pipeline& pipeline,
	const BufferView& cameraBufferView, const BufferView& particlesBufferView)
		: context(context), pipeline(pipeline) {

	createDescriptorPool(2, 2); // 2 сета, 2 storage-буфера суммарно

	descriptorSet = allocateDescriptorSet(*pipeline.getDescriptorSetLayout());
	computeDescriptorSet = allocateDescriptorSet(*pipeline.getComputeDescriptorSetLayout());

	vk::DescriptorBufferInfo uboInfo{cameraBufferView.buffer, 0, cameraBufferView.size};
	vk::DescriptorBufferInfo ssboInfo{particlesBufferView.buffer, 0, particlesBufferView.size};
	vk::DescriptorBufferInfo computeInfo{particlesBufferView.buffer, 0, particlesBufferView.size};

	std::array<vk::WriteDescriptorSet, 3> writes = {{
	    {.dstSet = *descriptorSet,
	     .dstBinding = 0,
	     .descriptorCount = 1,
	     .descriptorType = vk::DescriptorType::eUniformBuffer,
	     .pBufferInfo = &uboInfo},
	    {.dstSet = *descriptorSet,
	     .dstBinding = 1,
	     .descriptorCount = 1,
	     .descriptorType = vk::DescriptorType::eStorageBuffer,
	     .pBufferInfo = &ssboInfo},
	    {.dstSet = *computeDescriptorSet,
	     .dstBinding = 0,
	     .descriptorCount = 1,
	     .descriptorType = vk::DescriptorType::eStorageBuffer,
	     .pBufferInfo = &computeInfo},
	}};
	context.getDevice().updateDescriptorSets(writes, {});
	LOG_INFO("DescriptorPool::DescriptorPool", "Descriptor sets initialized");
}

vk::raii::DescriptorSet DescriptorPool::allocateDescriptorSet(vk::DescriptorSetLayout descriptorSetLayout) {
	vk::DescriptorSetAllocateInfo allocInfo{
	    .descriptorPool = descriptorPool, .descriptorSetCount = 1, .pSetLayouts = &descriptorSetLayout
	};

	auto sets = context.getDevice().allocateDescriptorSets(allocInfo);
	return std::move(sets.front());
}

void DescriptorPool::createDescriptorPool(uint32_t maxSets, uint32_t storageBufferCount) {
	std::array<vk::DescriptorPoolSize, 2> poolSizes = {{
	    {vk::DescriptorType::eUniformBuffer, 1},
	    {vk::DescriptorType::eStorageBuffer, storageBufferCount},
	}};
	vk::DescriptorPoolCreateInfo poolInfo{
	    .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
	    .maxSets = maxSets,
	    .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
	    .pPoolSizes = poolSizes.data(),
	};
	descriptorPool = vk::raii::DescriptorPool(context.getDevice(), poolInfo);
	LOG_INFO(
	    "DescriptorPool::createDescriptorPool",
	    "Descriptor pool created: max sets {}, storage buffers {}",
	    maxSets,
	    storageBufferCount
	);
}

void DescriptorPool::updateComputeSet(const BufferView& bufferView) {
	vk::DescriptorBufferInfo ssboInfo{bufferView.buffer, 0, bufferView.size};
	vk::WriteDescriptorSet write{
	    .dstSet = *computeDescriptorSet,
	    .dstBinding = 0,
	    .descriptorCount = 1,
	    .descriptorType = vk::DescriptorType::eStorageBuffer,
	    .pBufferInfo = &ssboInfo,
	};
	context.getDevice().updateDescriptorSets(write, {});
	LOG_INFO("DescriptorPool::updateComputeSet", "Compute descriptor set updated: buffer size {}", bufferView.size);
}

void DescriptorPool::updateGraphicsSet(const BufferView& bufferView) {
	vk::DescriptorBufferInfo ssboInfo{bufferView.buffer, 0, bufferView.size};
	vk::WriteDescriptorSet write{
	    .dstSet = *descriptorSet,
	    .dstBinding = 1, // binding=1 — particles SSBO в vertex шейдере
	    .descriptorCount = 1,
	    .descriptorType = vk::DescriptorType::eStorageBuffer,
	    .pBufferInfo = &ssboInfo,
	};
	context.getDevice().updateDescriptorSets(write, {});
	LOG_INFO("DescriptorPool::updateGraphicsSet", "Graphics descriptor set updated: buffer size {}", bufferView.size);
}