#ifndef GALACTIC_BUFFER_HPP
#define GALACTIC_BUFFER_HPP

#include "context.hpp"

class Buffer {
public:
	Buffer(
	    const VulkanContext& context,
	    vk::DeviceSize size,
	    vk::BufferUsageFlags usageFlags,
	    vk::MemoryPropertyFlags memPropertyFlags
	);

	void upload(const void* data, vk::DeviceSize uploadSize, vk::DeviceSize offset = 0);

	static Buffer createDeviceLocal(
	    const VulkanContext& context, const void* data, vk::DeviceSize size, vk::BufferUsageFlags usageFlags
	);

	vk::Buffer get() const { return *buffer; }
	vk::DeviceSize getSize() const { return size; }

private:
	vk::raii::Buffer createBuffer() const;
	vk::raii::DeviceMemory allocateMemory();
	std::uint32_t findMemoryType(std::uint32_t typeFilter, vk::MemoryPropertyFlags properties) const;

	const VulkanContext& context;
	vk::DeviceSize size = 0;
	vk::BufferUsageFlags usageFlags;
	vk::MemoryPropertyFlags memPropertyFlags;

	vk::raii::Buffer buffer = nullptr;
	vk::raii::DeviceMemory memory = nullptr;
	void* mappedPtr = nullptr;
};

#endif // GALACTIC_BUFFER_HPP