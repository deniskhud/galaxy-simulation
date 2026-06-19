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
    Pipeline(const VulkanContext& context, const SwapChain& swapChain) : context(context), swapChain(swapChain) {
        createDescriptorSetLayout();
        createPipeline();
        createComputePipeline();
    }
    const vk::raii::DescriptorSetLayout& getDescriptorSetLayout() const {
        return descriptorSetLayout;
    }

    const vk::raii::PipelineLayout& getPipelineLayout() const {
        return pipelineLayout;
    }
    const vk::raii::Pipeline& getPipeline() const {
        return pipeline;
    }

    const vk::raii::DescriptorSetLayout& getComputeDescriptorSetLayout() const { return computeDescriptorSetLayout; }
    const vk::raii::PipelineLayout& getComputePipelineLayout() const {
        return computePipelineLayout;
    }
    const vk::raii::Pipeline& getComputePipeline() const {
        return computePipeline;
    }
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

    [[nodiscard]] std::vector<char> readShaderFile(const std::string& filename) const;
    [[nodiscard]] vk::raii::ShaderModule createShaderModule(const std::vector<char> &code);

    bool createDescriptorSetLayout();
};
#endif //GALACTIC_PIPELINE_HPP