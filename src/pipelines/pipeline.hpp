#ifndef GALACTIC_PIPELINE_HPP
#define GALACTIC_PIPELINE_HPP

#include "../core/context.hpp"
#include "../core/swapchain.hpp"
#include <fstream>
class Pipeline {
public:
    Pipeline(const VulkanContext& context, const SwapChain& swapChain) : context(context), swapChain(swapChain) {  }
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
    void createComputePipeline();

    std::vector<char> readShaderFile(const std::string& filename) const;
    vk::raii::ShaderModule createShaderModule(const std::vector<char> &code);

    bool createDescriptorSetLayout();
};
#endif //GALACTIC_PIPELINE_HPP