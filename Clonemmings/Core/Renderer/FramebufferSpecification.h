#pragma once
#include <vector>
namespace Clonemmings
{
	enum class FramebufferTextureFormat
	{
		None = 0,

		RGBA8,
		RedInteger,

		Depth24Stencil8,

		Depth = Depth24Stencil8
	};

	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FramebufferTextureFormat format) : TextureFormat(format) {}

		FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
	};

	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments) : Attachments(attachments) {}

		std::vector<FramebufferTextureSpecification> Attachments;
	};
	struct FramebufferSpecification
	{
		uint32_t Width = 0;
		uint32_t Height = 0;
		uint32_t Samples = 1;
		FramebufferAttachmentSpecification Attachments;
	};
}
