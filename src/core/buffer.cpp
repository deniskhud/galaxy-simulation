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

	LOG_INFO("Buffer::Buffer", "Created buffer: size {} bytes", size);
}

Buffer::~Buffer() {
	if (mappedPtr) {
		memory.unmapMemory();
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
	LOG_ERROR("Buffer::findMemoryType", "Failed to find suitable memory type");
	return 0;
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
		LOG_ERROR("Buffer::upload", "Upload called on a buffer with no host-visible memory");
	}
	if (uploadSize + offset > size) {
		LOG_ERROR(
		    "Buffer::upload",
		    "Upload out of bounds: upload size {}, offset {}, buffer size {}",
		    uploadSize,
		    offset,
		    size
		);
	}
	std::memcpy(static_cast<char*>(mappedPtr) + offset, data, uploadSize);
}