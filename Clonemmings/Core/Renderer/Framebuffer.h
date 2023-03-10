#pragma once
#include "Core/Renderer/FramebufferSpecification.h"

namespace Clonemmings
{
	class FrameBuffer
	{
	public:
		FrameBuffer(const FramebufferSpecification& spec);
		~FrameBuffer();
		void Invalidate();
		void Bind() const;
		void Unbind() const;
		void Resize(uint32_t width, uint32_t height);
		int ReadPixel(uint32_t attachmentindex, int x, int y);
		uint32_t GetColourAttachmentHandle(uint32_t attachment) const;
		void ClearAttachment(uint32_t index, int value);
		const FramebufferSpecification& GetSpecification() const { return m_Specification; }

	private:
		uint32_t m_Handle = 0;
		FramebufferSpecification m_Specification;
		std::vector<FramebufferTextureSpecification> m_ColourAttachmentSpecifications;
		FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;
		std::vector<uint32_t> m_ColourAttachments;
		uint32_t m_DepthAttachment = 0;
	};
}
