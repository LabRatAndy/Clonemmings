#include "Core/Renderer/Framebuffer.h"
#include "Core/Application/Assert.h"
#include <glad.h>

namespace Clonemmings
{
	static const uint32_t S_MaxFrameBufferSize = 8192;

	namespace Utills
	{
		static GLenum TextureTarget(bool multisampled)
		{
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}
		static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count)
		{
			glCreateTextures(TextureTarget(multisampled), count, outID);
		}
		static void BindTexture(bool multisampled, uint32_t handle)
		{
			glBindTexture(TextureTarget(multisampled), handle);
		}
		static void AttachColourTexture(uint32_t handle, int samples, GLenum internalformat, GLenum format, uint32_t width, uint32_t height, int index)
		{
			//is it multisampled ie samples is more than 1
			if (samples > 1)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalformat, width, height, GL_FALSE);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(samples > 1), handle, 0);
		}
		static void AttachDepthTexture(uint32_t handle, int samples, GLenum format, GLenum attachmenttype, uint32_t width, uint32_t height)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmenttype, TextureTarget(multisampled), handle, 0);
		}
		static bool IsDepthFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
			case FramebufferTextureFormat::Depth24Stencil8: return true;
			}
			return false;
		}
		static GLenum FrameBufferTextureFormatToOpenGLFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
			case FramebufferTextureFormat::RGBA8: return GL_RGBA8;
			case FramebufferTextureFormat::RedInteger: return GL_RED_INTEGER;
			}
			ASSERT(false, "Framebuffer texture format not valid");
			return 0;
		}
	}
	FrameBuffer::FrameBuffer(const FramebufferSpecification& spec): m_Specification(spec)
	{
		for (auto spec : m_Specification.Attachments.Attachments)
		{
			if (!Utills::IsDepthFormat(spec.TextureFormat))
			{
				m_ColourAttachmentSpecifications.emplace_back(spec);
			}
			else
			{
				m_DepthAttachmentSpecification = spec;
			}
		}
		Invalidate();
	}
	FrameBuffer::~FrameBuffer()
	{
		//delete framebuffer
		glDeleteFramebuffers(1, &m_Handle);
		//delete colour attachments 
		glDeleteTextures(m_ColourAttachments.size(), m_ColourAttachments.data());
		//delete depth attachment
		glDeleteTextures(1, &m_DepthAttachment);
	}
	void FrameBuffer::Invalidate()
	{
		if (m_Handle)
		{
			//delete framebuffer
			glDeleteFramebuffers(1, &m_Handle);
			//delete colour attachments 
			glDeleteTextures(m_ColourAttachments.size(), m_ColourAttachments.data());
			//delete depth attachment
			glDeleteTextures(1, &m_DepthAttachment);
			m_ColourAttachments.clear();
			m_DepthAttachment = 0;
		}
		glCreateFramebuffers(1, &m_Handle);
		glBindFramebuffer(GL_FRAMEBUFFER, m_Handle);

		bool multisample = m_Specification.Samples > 1;

		if (m_ColourAttachmentSpecifications.size())
		{
			m_ColourAttachments.resize(m_ColourAttachmentSpecifications.size());
			Utills::CreateTextures(multisample, m_ColourAttachments.data(), m_ColourAttachments.size());
			for (size_t i = 0; i < m_ColourAttachments.size(); i++)
			{
				Utills::BindTexture(multisample, m_ColourAttachments[i]);
				switch (m_ColourAttachmentSpecifications[i].TextureFormat)
				{
				case FramebufferTextureFormat::RGBA8:
					Utills::AttachColourTexture(m_ColourAttachments[i], m_Specification.Samples, GL_RGBA8, GL_RGBA, m_Specification.Width, m_Specification.Height, i);
					break;
				case FramebufferTextureFormat::RedInteger:
					Utills::AttachColourTexture(m_ColourAttachments[i], m_Specification.Samples, GL_R32I, GL_RED_INTEGER, m_Specification.Width, m_Specification.Height, i);
					break;
				}
			}
		}
		if (m_DepthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None)
		{
			Utills::CreateTextures(multisample, &m_DepthAttachment, 1);
			Utills::BindTexture(multisample, m_DepthAttachment);
			switch (m_DepthAttachmentSpecification.TextureFormat)
			{
			case FramebufferTextureFormat::Depth24Stencil8:
				Utills::AttachDepthTexture(m_DepthAttachment, m_Specification.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, 
					m_Specification.Width, m_Specification.Height);
				break;
			}
		}
		if (m_ColourAttachments.size() >= 1)
		{
			ASSERT(m_ColourAttachments.size() <= 4);
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_ColourAttachments.size(), buffers);
		}
		else if (m_ColourAttachments.empty())
		{
			glDrawBuffer(GL_NONE);
		}
		ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
	void FrameBuffer::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_Handle);
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);
	}
	void FrameBuffer::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void FrameBuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > S_MaxFrameBufferSize || height > S_MaxFrameBufferSize)
		{
			WARN("Attempted to resize frame buffer to x: {0}, y: {1}", width, height);
			return;
		}
		m_Specification.Width = width;
		m_Specification.Height = height;
		Invalidate();
	}
	uint32_t FrameBuffer::GetColourAttachmentHandle(uint32_t attachment) const
	{
		ASSERT(attachment < m_ColourAttachments.size());
		return m_ColourAttachments[attachment];
	}
	int FrameBuffer::ReadPixel(uint32_t attachmentindex, int x, int y)
	{
		ASSERT(attachmentindex < m_ColourAttachments.size(), "invalid attachment index");
		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentindex);
		int pixeldata;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixeldata);
		return pixeldata;
	}
	void FrameBuffer::ClearAttachment(uint32_t index, int value)
	{
		ASSERT(index < m_ColourAttachments.size(), "invalid attachment index");
		auto& spec = m_ColourAttachmentSpecifications[index];
		glClearTexImage(m_ColourAttachments[index], 0, Utills::FrameBufferTextureFormatToOpenGLFormat(spec.TextureFormat), GL_INT, &value);
	}
}