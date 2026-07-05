#include "swapchain.hpp"

SwapChain::SwapChain(const VulkanContext& context, const Window& window) : context(context), window(window) {
	createSwapChain(context.getSurface());
	createImageViews();
}

vk::SurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const& availableFormats) {
	const auto formatIt = std::ranges::find_if(availableFormats, [](const auto& format) {
		return format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
	});
	if (formatIt == availableFormats.end()) {
		LOG_WARNING("SwapChain::chooseSwapSurfaceFormat", "Preferred surface format not found; using first available format");
	}
	return formatIt == availableFormats.end() ? availableFormats[0] : *formatIt;
}

vk::PresentModeKHR SwapChain::chooseSwapPresentMode(std::vector<vk::PresentModeKHR> const& availablePresentModes) {
	const bool supportsMailbox = std::ranges::any_of(
		       availablePresentModes,
		       [](const vk::PresentModeKHR value) { return vk::PresentModeKHR::eMailbox == value; }
		   );
	if (!supportsMailbox) {
		LOG_INFO("SwapChain::chooseSwapPresentMode", "Mailbox present mode not available; using FIFO");
	}
	return supportsMailbox ? vk::PresentModeKHR::eMailbox : vk::PresentModeKHR::eFifo;
}

vk::Extent2D SwapChain::chooseSwapExtent(vk::SurfaceCapabilitiesKHR const& surfaceCapabilities) {
	if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return surfaceCapabilities.currentExtent;
	}
	auto size = window.getFrameBufferSize();
	return {
	    std::clamp<std::uint32_t>(
	        size.first, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width
	    ),
	    std::clamp<std::uint32_t>(
	        size.second, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height
	    )
	};
}

void SwapChain::createSwapChain(const vk::raii::SurfaceKHR& surface) {
	vk::SurfaceCapabilitiesKHR surfaceCapabilities = context.getPhysicalDevice().getSurfaceCapabilitiesKHR(*surface);
	swapChainExtent = chooseSwapExtent(surfaceCapabilities);
	uint32_t minImageCount = chooseSwapMinImageCount(surfaceCapabilities);

	std::vector<vk::SurfaceFormatKHR> availableFormats = context.getPhysicalDevice().getSurfaceFormatsKHR(*surface);
	swapChainSurfaceFormat = chooseSwapSurfaceFormat(availableFormats);

	std::vector<vk::PresentModeKHR> availablePresentModes =
	    context.getPhysicalDevice().getSurfacePresentModesKHR(*surface);
	vk::PresentModeKHR presentMode = chooseSwapPresentMode(availablePresentModes);

	vk::SwapchainCreateInfoKHR swapChainCreateInfo = {
	    .surface = *surface,
	    .minImageCount = minImageCount,
	    .imageFormat = swapChainSurfaceFormat.format,
	    .imageColorSpace = swapChainSurfaceFormat.colorSpace,
	    .imageExtent = swapChainExtent,
	    .imageArrayLayers = 1,
	    .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
	    .imageSharingMode = vk::SharingMode::eExclusive,
	    .preTransform = surfaceCapabilities.currentTransform,
	    .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
	    .presentMode = presentMode,
	    .clipped = true,
	    .oldSwapchain = nullptr
	};
	swapChain = vk::raii::SwapchainKHR(context.getDevice(), swapChainCreateInfo);
	swapChainImages = swapChain.getImages();
	LOG_INFO(
	    "SwapChain::createSwapChain",
	    "Swapchain created: {}x{}, images {}",
	    swapChainExtent.width,
	    swapChainExtent.height,
	    swapChainImages.size()
	);
}

void SwapChain::recreateSwapChain() {
	auto windowSize = window.getFrameBufferSize();
	while (windowSize.first == 0 || windowSize.second == 0) {
		windowSize = window.getFrameBufferSize();
	}
	context.getDevice().waitIdle();
	cleanupSwapChain();
	createSwapChain(context.getSurface());
	createImageViews();
	LOG_INFO("SwapChain::recreateSwapChain", "Swapchain recreated");
}
void SwapChain::cleanupSwapChain() {
	imageViews.clear();
	swapChain = nullptr;
}

uint32_t SwapChain::chooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const& surfaceCapabilities) {
	auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);
	if ((0 < surfaceCapabilities.maxImageCount) && (surfaceCapabilities.maxImageCount < minImageCount)) {
		minImageCount = surfaceCapabilities.maxImageCount;
	}
	return minImageCount;
}

void SwapChain::createImageViews() {
	vk::ImageViewCreateInfo imageViewCreateInfo = {
	    .viewType = vk::ImageViewType::e2D,
	    .format = swapChainSurfaceFormat.format,
	};
	imageViewCreateInfo.components = {
	    vk::ComponentSwizzle::eIdentity,
	    vk::ComponentSwizzle::eIdentity,
	    vk::ComponentSwizzle::eIdentity,
	    vk::ComponentSwizzle::eIdentity,
	};
	imageViewCreateInfo.subresourceRange = {
	    .aspectMask = vk::ImageAspectFlagBits::eColor, .levelCount = 1, .layerCount = 1
	};

	for (auto& image : swapChainImages) {
		imageViewCreateInfo.image = image;
		imageViews.emplace_back(vk::raii::ImageView(context.getDevice(), imageViewCreateInfo));
	}
}

/*** getters ***/
vk::Format SwapChain::getSwapChainImageFormat() const {
	return swapChainSurfaceFormat.format;
}
const vk::raii::SwapchainKHR& SwapChain::getSwapchain() const {
	return swapChain;
}
const vk::Extent2D& SwapChain::getExtent() const {
	return swapChainExtent;
}
const vk::raii::ImageView& SwapChain::getImageView(std::uint32_t index) const {
	return imageViews.at(index);
}
const vk::Image& SwapChain::getImage(std::uint32_t index) const {
	return swapChainImages.at(index);
}

const size_t SwapChain::getSwapChainImageCount() const {
	return swapChainImages.size();
}