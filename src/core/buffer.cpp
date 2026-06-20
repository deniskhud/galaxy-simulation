#include "buffer.hpp"

Buffer::Buffer(const VulkanContext& context, vk::DeviceSize size,
               vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags memPropertyFlags)
    : context(context)
    , size(size)
    , usageFlags(usageFlags)
    , memPropertyFlags(memPropertyFlags)
{
    buffer = createBuffer();
    memory = allocateMemory();
    buffer.bindMemory(*memory, 0);

    if (memPropertyFlags & vk::MemoryPropertyFlagBits::eHostVisible) {
        mappedPtr = memory.mapMemory(0, size);
    }
}

vk::raii::Buffer Buffer::createBuffer() const {
    vk::BufferCreateInfo bufferInfo{
        .size = size,
        .usage = usageFlags,
        .sharingMode = vk::SharingMode::eExclusive,
    };
    return vk::raii::Buffer(context.getDevice(), bufferInfo);
}

std::uint32_t Buffer::findMemoryType(std::uint32_t typeFilter, vk::MemoryPropertyFlags properties) const {
    vk::PhysicalDeviceMemoryProperties memProps = context.getPhysicalDevice().getMemoryProperties();
    for (std::uint32_t i = 0; i < memProps.memoryTypeCount; ++i) {
        if ((typeFilter & (1u << i)) &&
            (memProps.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    throw std::runtime_error("Failed to find suitable memory type");
}

vk::raii::DeviceMemory Buffer::allocateMemory() {
    vk::MemoryRequirements memReq = buffer.getMemoryRequirements();
    vk::MemoryAllocateInfo allocInfo{
        .allocationSize = memReq.size,
        .memoryTypeIndex = findMemoryType(memReq.memoryTypeBits, memPropertyFlags),
    };
    return vk::raii::DeviceMemory(context.getDevice(), allocInfo);
}

void Buffer::upload(const void* data, vk::DeviceSize uploadSize, vk::DeviceSize offset) {
    if (!mappedPtr) {
        throw std::runtime_error("Buffer: upload is called on a buffer with no host-visible memory");
    }
    std::memcpy(static_cast<char*>(mappedPtr) + offset, data, uploadSize);
}

Buffer Buffer::createDeviceLocal(const VulkanContext& context, const void* data,
                                  vk::DeviceSize size, vk::BufferUsageFlags usageFlags) {
    Buffer staging(context, size, vk::BufferUsageFlagBits::eTransferSrc,
                   vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    staging.upload(data, size);

    Buffer deviceLocal(context, size, usageFlags | vk::BufferUsageFlagBits::eTransferDst,
                        vk::MemoryPropertyFlagBits::eDeviceLocal);

    vk::CommandPoolCreateInfo poolInfo{
        .flags = vk::CommandPoolCreateFlagBits::eTransient,
        .queueFamilyIndex = context.getGraphicsQueueFamilyIndex(),
    };
    vk::raii::CommandPool transientPool(context.getDevice(), poolInfo);

    vk::CommandBufferAllocateInfo cmdAllocInfo{
        .commandPool = *transientPool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1,
    };
    auto cmdBuffers = context.getDevice().allocateCommandBuffers(cmdAllocInfo);
    vk::raii::CommandBuffer cmd = std::move(cmdBuffers.front());

    cmd.begin(vk::CommandBufferBeginInfo{.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    vk::BufferCopy copyRegion{.size = size};
    cmd.copyBuffer(*staging.buffer, *deviceLocal.buffer, copyRegion);
    cmd.end();

    vk::SubmitInfo submitInfo{
        .commandBufferCount = 1,
        .pCommandBuffers = &*cmd,
    };
    context.getGraphicsQueue().submit(submitInfo);
    context.getGraphicsQueue().waitIdle();

    return deviceLocal;
}