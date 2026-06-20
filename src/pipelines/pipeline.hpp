#ifndef GALACTIC_PIPELINE_HPP
#define GALACTIC_PIPELINE_HPP

#include "../core/context.hpp"
#include "../core/swapchain.hpp"
#include <fstream>

struct SimParams {
    float time;
    float maxOrbitalSpeed;
    float coreRadius;
    uint32_t particleCount;
};

class Pipeline {
public:
    Pipeline(const VulkanContext& context, const SwapChain& swapChain);

    const vk::raii::DescriptorSetLayout& getDescriptorSetLayout() const;

    const vk::raii::PipelineLayout& getPipelineLayout() const;
    const vk::raii::Pipeline& getPipeline() const;
    const vk::raii::DescriptorSetLayout& getComputeDescriptorSetLayout() const;
    const vk::raii::PipelineLayout& getComputePipelineLayout() const;
    const vk::raii::Pipeline& getComputePipeline() const;
private:
    const VulkanContext& context;
    const SwapChain& swapChain;

    // pipeline
    vk::raii::Pipeline pipeline = nullptr;
    vk::raii::PipelineLayout pipelineLayout = nullptr;
    vk::raii::DescriptorSetLayout descriptorSetLayout = nullptr;

    // compute pipeline
    vk::raii::Pipeline computePipeline = nullptr;
    vk::raii::PipelineLayout computePipelineLayout = nullptr;
    vk::raii::DescriptorSetLayout computeDescriptorSetLayout = nullptr;

    bool createPipeline();
    bool createComputePipeline();

    [[nodiscard]] static std::vector<char> readShaderFile(const std::string& filename);
    [[nodiscard]] vk::raii::ShaderModule createShaderModule(const std::vector<char> &code) const;

    bool createDescriptorSetLayout();
};
#endif //GALACTIC_PIPELINE_HPP