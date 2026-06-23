#include "descriptors.hpp"

DescriptorPool::DescriptorPool(
    const VulkanContext& context,
    const Pipeline& pipeline,
    vk::Buffer cameraUbo,
    vk::DeviceSize uboSize,
    vk::Buffer particleSsbo,
    vk::DeviceSize ssboSize
)
    : context(context), pipeline(pipeline) {
	createDescriptorPool(1, 1);
	descriptorSet = allocateDescriptorSet(*pipeline.getDescriptorSetLayout());

	vk::DescriptorBufferInfo uboInfo{cameraUbo, 0, uboSize};
	vk::DescriptorBufferInfo ssboInfo{particleSsbo, 0, ssboSize};
	std::array<vk::WriteDescriptorSet, 2> writes = {{
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
	}};
	context.getDevice().updateDescriptorSets(writes, {});
}

DescriptorPool::DescriptorPool(
    const VulkanContext& context,
    const Pipeline& pipeline,
    vk::Buffer cameraUbo,
    vk::DeviceSize uboSize,
    vk::Buffer particleSsbo,
    vk::DeviceSize ssboSize,
    vk::Buffer computeBuffer,
    vk::DeviceSize computeBufferSize
)
    : context(context), pipeline(pipeline) {
	createDescriptorPool(2, 2); // 2 сета, 2 storage-буфера суммарно

	descriptorSet = allocateDescriptorSet(*pipeline.getDescriptorSetLayout());
	computeDescriptorSet = allocateDescriptorSet(*pipeline.getComputeDescriptorSetLayout());

	vk::DescriptorBufferInfo uboInfo{cameraUbo, 0, uboSize};
	vk::DescriptorBufferInfo ssboInfo{particleSsbo, 0, ssboSize};
	vk::DescriptorBufferInfo computeInfo{computeBuffer, 0, computeBufferSize};

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
}