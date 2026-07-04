#include "buffer.hpp"

Buffer::Buffer(
    const VulkanContext& context,
    vk::DeviceSize size,
    vk::BufferUsageFlags usageFlags,
    vk::MemoryPropertyFlags memPropertyFlags
)
    : context(context), size(size) {
	buffer = createBuffer(usageFlags);
	memory = allocateMemory(memPropertyFlags);
	buffer.bindMemory(*memory, 0);

	if (memPropertyFlags & vk::MemoryPropertyFlagBits::eHostVisible) {
		mappedPtr = memory.mapMemory(0, size);
	}
}

vk::raii::Buffer Buffer::createBuffer(const vk::BufferUsageFlags& usageFlags) const {
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
		if ((typeFilter & (1u << i)) && (memProps.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
	throw std::runtime_error("Failed to find suitable memory type");
}

vk::raii::DeviceMemory Buffer::allocateMemory(const vk::MemoryPropertyFlags& memPropertyFlags) {
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
	if (uploadSize + offset > size) {
		throw std::runtime_error("Buffer: upload size is too big");
	}
	std::memcpy(static_cast<char*>(mappedPtr) + offset, data, uploadSize);
}