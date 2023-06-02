/*MIT License

Copyright(c) 2022, Andrew Dawson

Permission is hereby granted, free of charge, to any person obtaining a copy
of this softwareand associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/
#pragma once
#include "Core/Renderer/Shader.h"
#include "Core/Renderer/VertexArray.h"
#include "Core/Renderer/Texture.h"
#include "Core/Renderer/SceneCamera.h"
#include <glm/glm.hpp>
#include <memory>
#include <filesystem>
namespace Clonemmings
{
	struct RendererSetupData
	{
		size_t MaxQuads = 1000;
		size_t MaxTextures = 32;
		size_t MaxLines = 100;
		std::filesystem::path BatchVertexShaderFilename;
		std::filesystem::path BatchFragmentShaderFilename;
		std::filesystem::path TexturedVertexShaderFilename;
		std::filesystem::path TexturedFragmentShaderFilename;
		std::filesystem::path ColouredVertexShaderFilename;
		std::filesystem::path ColouredFragmentShaderFilename;
		std::filesystem::path LineVertexShaderFilename;
		std::filesystem::path LineFragmentShaderFilename;
	};
	enum class BatchType
	{
		None = 0,
		Quad,
		Line
	};
	class Renderer
	{
	public:
		Renderer(RendererSetupData setupdata);
		~Renderer();

		void SetCamera(SceneCamera*camera, const glm::mat4& transform) { m_Camera = camera; m_CameraTransform = transform; }
		SceneCamera* GetCamera() { return m_Camera; }

		//non batched renderering functions
		void DrawColouredNonIndexed(const VertexArrayObject& vao, const glm::mat4& modeltransform);
		void DrawTexturedNonIndexed(const VertexArrayObject& vao, const glm::mat4& modeltransform, std::shared_ptr<Texture> texture);
		void DrawColouredIndexed(const VertexArrayObject& vao, const glm::mat4& modeltransform);
		void DrawTexturedIndexed(const VertexArrayObject& vao, const glm::mat4& modeltransform, std::shared_ptr<Texture> texture);

		//Batched Renderering functions
		void StartBatch(const BatchType batchtype);
		void DrawBatchedQuad(const glm::vec3& position, const glm::vec2& size, std::shared_ptr<Texture> texture, const glm::vec4& colour, float tilingfactor = 1.0f,
			int entityID = -1);
		void DrawBatchedRotatedQuad(const glm::vec3& position, const glm::vec2& size, std::shared_ptr<Texture> texture, const glm::vec4& colour, float roatation = 0.0f,
			float tilingfactor = 1.0f, int entityID = -1);
		void DrawLine(const glm::vec3& startpos, const glm::vec3& endpos, const glm::vec4& Colour, int entityID = -1);
		void DrawRectangle(const glm::mat4& transform, const glm::vec4& colour, int entityID = -1);
		void EndBatch(const BatchType batchtype);

		//General renderer functions
		void Clear();
		void SetClearColour(const glm::vec4& colour);
		void SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		void SetDepthTest(bool enable);
		void SetBlending(bool enable);
		void SetBackFaceCull(bool enable);
		void SetWindingOrderClockwise();
		void SetWindingOrderAntiClockwise();
		float GetLineWidth() { return m_LineWidth; }
		void SetLineWidth(float linewidth) { m_LineWidth = linewidth; }
		void SetLineSmooth(bool enable);


	private:
		//shaders
		std::unique_ptr<Shader> m_TexturedShader = nullptr;
		std::unique_ptr<Shader> m_ColouredShader = nullptr;
		std::unique_ptr<Shader> m_BatchShader = nullptr;
		std::unique_ptr<Shader> m_LineShader = nullptr;

		//Renderer Settings
		bool m_DepthTestOn = false;
		bool m_BlendingOn = false;
		bool m_BackFaceCull = false;
		bool m_ClockwiseWinding = false;
		bool m_LineSmooth = false;

		void GetInitalDefaults();

		//batch rendering  stuff
		const size_t m_MaxQuads;
		const size_t m_MaxVertices = m_MaxQuads * 4;
		const size_t m_MaxIndices = m_MaxQuads * 6;
		const size_t m_MaxTextures;
		glm::vec4 m_QuadVertices[4];
		glm::vec2 m_TexCoords[4];
		std::shared_ptr<VertexBufferObject> m_BatchVBO = nullptr;
		std::shared_ptr<IndexBuffer> m_BatchIBO = nullptr;
		std::unique_ptr<VertexArrayObject> m_BatchVAO = nullptr;
		std::vector<std::shared_ptr<Texture>> m_Textures;
		size_t m_QuadCount = 0;
		uint32_t m_TextureCount = 0;
		BatchedVertex* m_CurrentVertex = nullptr;
		std::shared_ptr<Texture> m_WhiteTexture = nullptr;
		//line renderering variables
		const size_t m_MaxLinesVertices;
		std::shared_ptr<VertexBufferObject> m_LineVBO = nullptr;
		std::shared_ptr<VertexArrayObject> m_LineVAO = nullptr;
		float m_LineWidth = 2.0f;
		uint32_t m_LineVertexCount = 0;
		LineVertex* m_CurrentLineVertex = nullptr;

		void SubmitToBatch(const glm::mat4& transform, std::shared_ptr<Texture> texture, const glm::vec4& colour, float tilingfactor, int entityID);
		//Camera
		SceneCamera* m_Camera = nullptr;
		glm::mat4 m_CameraTransform;
	};
}