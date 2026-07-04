#ifndef GALACTIC_BUFFER_HPP
#define GALACTIC_BUFFER_HPP

#include "context.hpp"

struct BufferView {
	vk::Buffer buffer{nullptr};
	vk::DeviceSize size{0};
};

class Buffer {
public:
	Buffer(
	    const VulkanContext& context,
	    vk::DeviceSize size,
	    vk::BufferUsageFlags usageFlags,
	    vk::MemoryPropertyFlags memPropertyFlags
	);

	void upload(const void* data, vk::DeviceSize uploadSize, vk::DeviceSize offset = 0);

	BufferView getBufferView() const {
		return {*buffer, size};
	}

private:
	vk::raii::Buffer createBuffer(const vk::BufferUsageFlags& usageFlags) const;
	vk::raii::DeviceMemory allocateMemory(const vk::MemoryPropertyFlags& memPropertyFlags);
	std::uint32_t findMemoryType(std::uint32_t typeFilter, vk::MemoryPropertyFlags properties) const;

	const VulkanContext& context;
	vk::raii::Buffer buffer{nullptr};
	vk::DeviceSize size{0};

	vk::raii::DeviceMemory memory{nullptr};
	void* mappedPtr{nullptr};
};
#endif // GALACTIC_BUFFER_HPP