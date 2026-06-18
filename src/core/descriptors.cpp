#include "descriptors.hpp"

DescriptorPool::DescriptorPool(const VulkanContext& context, const Pipeline& pipeline,
                                vk::Buffer cameraUbo, vk::DeviceSize uboSize,
                                vk::Buffer particleSsbo, vk::DeviceSize ssboSize)
    : context(context), pipeline(pipeline)
{
    createDescriptorPool();
    descriptorSet = allocateDescriptorSet();

    vk::DescriptorBufferInfo uboInfo  { cameraUbo,    0, uboSize  };
    vk::DescriptorBufferInfo ssboInfo { particleSsbo, 0, ssboSize };

    std::array<vk::WriteDescriptorSet, 2> writes = {{
        { .dstSet = *descriptorSet, .dstBinding = 0, .descriptorCount = 1,
          .descriptorType = vk::DescriptorType::eUniformBuffer,  .pBufferInfo = &uboInfo  },
        { .dstSet = *descriptorSet, .dstBinding = 1, .descriptorCount = 1,
          .descriptorType = vk::DescriptorType::eStorageBuffer,  .pBufferInfo = &ssboInfo },
    }};
    context.getDevice().updateDescriptorSets(writes, {});
}

vk::raii::DescriptorSet DescriptorPool::allocateDescriptorSet() {
    vk::DescriptorSetAllocateInfo allocInfo{
        .descriptorPool = descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &*pipeline.getDescriptorSetLayout()
    };

    auto sets = context.getDevice().allocateDescriptorSets(allocInfo);
    return std::move(sets.front());
}

void DescriptorPool::createDescriptorPool() {
    std::array<vk::DescriptorPoolSize, 2> poolSizes = {{
        { vk::DescriptorType::eUniformBuffer, 1 },
        { vk::DescriptorType::eStorageBuffer, 1 },
    }};

    vk::DescriptorPoolCreateInfo poolInfo{
        .maxSets = 1,
        .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
        .pPoolSizes = poolSizes.data(),
    };

    descriptorPool = vk::raii::DescriptorPool(context.getDevice(), poolInfo);
}
