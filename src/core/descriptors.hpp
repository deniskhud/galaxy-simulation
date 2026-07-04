#ifndef GALACTIC_DESCRIPTORS_HPP
#define GALACTIC_DESCRIPTORS_HPP
#include "../pipelines/pipeline.hpp"
#include "context.hpp"

#include <array>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include "../core/buffer.hpp"
class DescriptorPool {
public:
	DescriptorPool(
	    const VulkanContext& context,
	    const Pipeline& pipeline,
	    const BufferView& cameraBufferView,
	    const BufferView& particlesBufferView
	);

	const vk::raii::DescriptorSet& get() const { return descriptorSet; }
	const vk::raii::DescriptorSet& getComputeSet() const { return computeDescriptorSet; }

	void updateComputeSet(const BufferView& bufferView);
	void updateGraphicsSet(const BufferView& bufferView);

private:
	vk::raii::DescriptorPool descriptorPool = nullptr;
	vk::raii::DescriptorSet descriptorSet = nullptr;

	vk::raii::DescriptorSet computeDescriptorSet = nullptr;

	const VulkanContext& context;
	const Pipeline& pipeline;

	void createDescriptorPool(uint32_t maxSets, uint32_t storageBufferCount);
	vk::raii::DescriptorSet allocateDescriptorSet(vk::DescriptorSetLayout descriptorSetLayout);
};
#endif // GALACTIC_DESCRIPTORS_HPP