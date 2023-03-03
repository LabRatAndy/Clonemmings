#include "Game/GameLayer.h"
#include "Core/Application/Application.h"
#include "Core/Renderer/Renderer.h"

#include <imgui.h>
namespace Clonemmings
{
	GameLayer::GameLayer(const std::string& name) : Layer(name)
	{

	}
	void GameLayer::OnAttach()
	{
#if 0
		ColouredVertex* vertices = new ColouredVertex[6];
		vertices[0].Position = { -0.5,-0.5,0.0 };
		vertices[1].Position = { 0.5,-0.5,0.0 };
		vertices[2].Position = { 0.5,0.5,0.0 };
		vertices[3].Position = { 0.5,0.5,0.0 };
		vertices[4].Position = { -0.5,0.5,0.0 };
		vertices[5].Position = { -0.5,-0.5,0.0 };
		vertices[0].Normal = { 0.0,0.0,0.0 };
		vertices[1].Normal = { 0.0,0.0,0.0 };
		vertices[2].Normal = { 0.0,0.0,0.0 };
		vertices[3].Normal = { 0.0,0.0,0.0 };
		vertices[4].Normal = { 0.0,0.0,0.0 };
		vertices[5].Normal = { 0.0,0.0,0.0 };
		vertices[0].Colour = { 1.0,1.0,1.0,1.0 };
		vertices[1].Colour = { 1.0,1.0,1.0,1.0 }; 
		vertices[2].Colour = { 1.0,1.0,1.0,1.0 };
		vertices[3].Colour = { 1.0,1.0,1.0,1.0 };
		vertices[4].Colour = { 1.0,1.0,1.0,1.0 };
		vertices[5].Colour = { 1.0,1.0,1.0,1.0 };
		uint32_t* indices = new uint32_t[6];
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;
		indices[3] = 2;
		indices[4] = 3;
		indices[5] = 0;
		m_VAO = std::make_shared<VertexArrayObject>();
		m_VAO->Bind();
		m_VBO = std::make_shared<VertexBufferObject>(vertices, 6 * sizeof(ColouredVertex), VertexType::Coloured);
		m_IBO = std::make_shared<IndexBuffer>(indices, 6);
		m_VAO->AddVertexBuffer(m_VBO);
		m_VAO->SetIndexBuffer(m_IBO);
		m_VAO->UnBind();
		delete[] vertices;
		delete[] indices;
#elif 0
		TexturedVertex* vertices = new TexturedVertex[4];
		vertices[0].Position = { -0.5,-0.5,0.0 };
		vertices[1].Position = { 0.5,-0.5,0.0 };
		vertices[2].Position = { 0.5,0.5,0.0 };
		vertices[3].Position = { -0.5,0.5,0.0 };
		vertices[0].Normal = { 0.0,0.0,0.0 };
		vertices[1].Normal = { 0.0,0.0,0.0 };
		vertices[2].Normal = { 0.0,0.0,0.0 };
		vertices[3].Normal = { 0.0,0.0,0.0 };
		vertices[0].TexCoords = { 0.0,0.0 };
		vertices[1].TexCoords = { 1.0,0.0 };
		vertices[2].TexCoords = { 1.0,1.0 };
		vertices[3].TexCoords = { 0.0,1.0 };
		uint32_t* indices = new uint32_t[6];
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;
		indices[3] = 2;
		indices[4] = 3;
		indices[5] = 0;
		m_VAO = std::make_shared<VertexArrayObject>();
		m_VAO->Bind();
		m_VBO = std::make_shared<VertexBufferObject>(vertices, 4 * sizeof(TexturedVertex), VertexType::Textured);
		m_IBO = std::make_shared<IndexBuffer>(indices, 6);
		m_VAO->AddVertexBuffer(m_VBO);
		m_VAO->SetIndexBuffer(m_IBO);
		m_VAO->UnBind();
		delete[] vertices;
		delete[] indices;
		glm::vec4 colour;
		colour.r = 0.0;
		colour.g = 1.0;
		colour.b = 0.0;
		colour.a = 1.0;
		m_Texture = std::make_shared<Texture>(4, 4, glm::vec4(0.0, 1.0, 0.0, 1.0));
#else
		m_Texture = std::make_shared<Texture>("Assets/Textures/test.png");
#endif
		//framebuffer set up for dockspace viewport
		FramebufferSpecification spec;
		spec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::Depth };
		// set width and height as 0 causes a crash in depth attachment.
		spec.Width = Application::Get().GetWindow().GetWidth();
		spec.Height = Application::Get().GetWindow().GetHeight();
		m_Framebuffer = std::make_shared<FrameBuffer>(spec);
	}
	void GameLayer::OnDetach()
	{

	}
	void GameLayer::OnUpdate(TimeStep ts)
	{
		//bind the frame buffer before rendering
		m_Framebuffer->Bind();
		Application::Get().GetRenderer().Clear();
		Application::Get().GetRenderer().StartBatch();
		Application::Get().GetRenderer().DrawBatchedQuad(glm::vec3(-5.0, 10.0, 0.0), glm::vec2(100.0, 250.0), nullptr, glm::vec4(0.0, 1.0, 1.0, 1.0));
		Application::Get().GetRenderer().DrawBatchedQuad(glm::vec3(150.0, 0.0, 0.0), glm::vec2(50.0, 50.0), m_Texture, glm::vec4(1.0));
		Application::Get().GetRenderer().EndBatch();
		m_Framebuffer->Unbind();
	}
	void GameLayer::OnImGuiRender()
	{
		// dock space set up taken from ImGui examples!

		// Note: Switch this to true to enable dockspace
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f,0.0f });
		ImGui::Begin("Dockspace demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();
		if (opt_fullscreen)
		{
			ImGui::PopStyleVar(2);
		}

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		//our code goes here
		//menu bar
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Close")) Application::Get().Close();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		//viewport window
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0, 0.0));
		ImGui::Begin("Viewport");
		ImVec2 viewportpanelsize = ImGui::GetContentRegionAvail();
		if (m_ViewportSize != *((glm::vec2*)&viewportpanelsize))
		{
			m_Framebuffer->Resize((uint32_t)viewportpanelsize.x, (uint32_t)viewportpanelsize.y);
			m_ViewportSize = { viewportpanelsize.x,viewportpanelsize.y };
			Application::Get().GetRenderer().GetCamera()->SetOrthographicSize(viewportpanelsize.y);
			Application::Get().GetRenderer().GetCamera()->SetViewportSize((uint32_t)viewportpanelsize.x, (uint32_t)viewportpanelsize.y);
		}
		uint32_t texturehandle = m_Framebuffer->GetColourAttachmentHandle(0);
		ImGui::Image((void*)texturehandle, ImVec2{ m_ViewportSize.x,m_ViewportSize.y }, ImVec2{ 0,1 }, ImVec2{ 1,0 });
		ImGui::End();
		ImGui::PopStyleVar();

		//end dockspace
		ImGui::End();
	}
}