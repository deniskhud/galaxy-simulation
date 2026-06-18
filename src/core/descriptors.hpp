#ifndef GALACTIC_DESCRIPTORS_HPP
#define GALACTIC_DESCRIPTORS_HPP
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan.hpp>
#include <array>
#include "context.hpp"
#include "../pipelines/pipeline.hpp"
class DescriptorPool {
public:
    void createDescriptorPool();
    vk::raii::DescriptorSet allocateDescriptorSet();

    DescriptorPool(const VulkanContext& context, const Pipeline& pipeline,
                   vk::Buffer cameraUbo, vk::DeviceSize uboSize,
                   vk::Buffer particleSsbo, vk::DeviceSize ssboSize);


    const vk::raii::DescriptorSet& get() const {
        return descriptorSet;
    }
private:
    vk::raii::DescriptorPool descriptorPool = nullptr;
    vk::raii::DescriptorSet descriptorSet = nullptr;

    const VulkanContext& context;
    const Pipeline& pipeline;
};



#endif //GALACTIC_DESCRIPTORS_HPP
