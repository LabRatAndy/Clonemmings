#include "Game/GameLayer.h"
#include "Core/Application/Application.h"
#include "Core/Renderer/Renderer.h"
#include "Core/Scene/Entity.h"

#include <imgui.h>
namespace Clonemmings
{
	GameLayer::GameLayer(const std::string& name) : Layer(name)
	{

	}
	void GameLayer::OnAttach()
	{

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
		m_ActiveScene->OnUpdateRuntime(ts);
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
	void GameLayer::SetScene(std::shared_ptr<Scene> scene)
	{
		m_ActiveScene = scene;
		//temp code for testing
		{
			Entity entity = m_ActiveScene->CreateEntity("Blue Rectangle");
			auto& transform = entity.GetComponent<TransformComponent>();
			transform.Translation.x = -5.0;
			transform.Translation.y = 10.0;
			transform.Translation.z = 0.0;
			transform.Rotation.x = 0.0;
			transform.Rotation.y = 0.0;
			transform.Rotation.z = 0.0;
			transform.Scale.x = 100.0;
			transform.Scale.y = 250.0;
			transform.Scale.z = 1.0;
			auto& src = entity.AddComponent<SpriteRendererComponent>(glm::vec4(0.0, 1.0, 1.0, 1.0));
			src.TilingFactor = 1;
			src.Tex = nullptr;
		}
		{
			Entity entity = m_ActiveScene->CreateEntity("Textured square");
			auto& transform = entity.GetComponent<TransformComponent>();
			transform.Translation.x = 150.0;
			transform.Translation.y = 0.0;
			transform.Translation.z = 0.0;
			transform.Rotation.x = 0.0;
			transform.Rotation.y = 0.0;
			transform.Rotation.z = 0.0;
			transform.Scale.x = 50.0;
			transform.Scale.y = 50.0;
			transform.Scale.z = 1.0;
			auto& src = entity.AddComponent<SpriteRendererComponent>();
			src.Colour = glm::vec4(1.0);
			src.TilingFactor = 1;
			src.Tex = std::make_shared<Texture>("Assets/Textures/test.png");
 		}
		{
			Entity entity = m_ActiveScene->CreateEntity("Camera");
			auto& transform = entity.GetComponent<TransformComponent>();
			transform.Translation.x = 0.0;
			transform.Translation.y = 0.0;
			transform.Translation.z = -1.0;
			transform.Rotation.x = 0.0;
			transform.Rotation.y = 0.0;
			transform.Rotation.z = 0.0;
			transform.Scale.x = 1.0;
			transform.Scale.y = 1.0;
			transform.Scale.z = 1.0;
			auto& camera = entity.AddComponent<CameraComponent>();
			camera.Camera = *(Application::Get().GetRenderer().GetCamera());
			camera.FixedAspectRatio = false;
			camera.Primary = true;
		}
		
	}
}