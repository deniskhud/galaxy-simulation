#ifndef GALACTIC_DESCRIPTORS_HPP
#define GALACTIC_DESCRIPTORS_HPP
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan.hpp>
#include <array>
#include "context.hpp"
#include "../pipelines/pipeline.hpp"
class DescriptorPool {
public:
    void createDescriptorPool(uint32_t maxSets, uint32_t storageBufferCount);
    vk::raii::DescriptorSet allocateDescriptorSet(vk::DescriptorSetLayout descriptorSetLayout);

    DescriptorPool(const VulkanContext& context, const Pipeline& pipeline,
                   vk::Buffer cameraUbo, vk::DeviceSize uboSize,
                   vk::Buffer particleSsbo, vk::DeviceSize ssboSize);

    DescriptorPool(const VulkanContext& context, const Pipeline& pipeline,
                   vk::Buffer cameraUbo, vk::DeviceSize uboSize,
                   vk::Buffer particleSsbo, vk::DeviceSize ssboSize, vk::Buffer computeBuffer, vk::DeviceSize computeBufferSize);


    const vk::raii::DescriptorSet& get() const {
        return descriptorSet;
    }
    const vk::raii::DescriptorSet& getComputeSet() const {
        return computeDescriptorSet;
    }
private:
    vk::raii::DescriptorPool descriptorPool = nullptr;
    vk::raii::DescriptorSet descriptorSet = nullptr;

    vk::raii::DescriptorSet computeDescriptorSet = nullptr;

    const VulkanContext& context;
    const Pipeline& pipeline;
};
#endif //GALACTIC_DESCRIPTORS_HPP