#include "Game/GameLayer.h"
#include "Core/Application/Application.h"
#include "Core/Renderer/Renderer.h"
#include "Core/Scene/Entity.h"
#include "Core/Scene/SceneSerialiser.h"
#include "Core/Application/FileDialog.h"
#include "Core/Scene/CoreComponents.h"
#include "Core/Application/Input.h"
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
		spec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RedInteger, FramebufferTextureFormat::Depth };
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
		//Note don't remove the clear command from here as it needs to be here to draw the scene correctly. Not sure of the reason why but possibly due to framebuffer use.
		Application::Get().GetRenderer().Clear();
		m_Framebuffer->ClearAttachment(1, -1);
		m_ActiveScene->OnUpdateRuntime(ts);
		
		//get selected entity
		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewportsize = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = viewportsize.y - my;
		int mousex = (int)mx;
		int mousey = (int)my;
		if (mousex >= 0 && mousey >= 0 && mousex < (int)viewportsize.x && mousey < (int)viewportsize.y)
		{
			int pixeldata = m_Framebuffer->ReadPixel(1, mousex, mousey);
			//TRACE("Hovered entity: {}", pixeldata);
			if (pixeldata != -1)
			{
				
				m_HoveredEntity = Entity((entt::entity)pixeldata, m_ActiveScene.get());
			}
			else
			{
				m_HoveredEntity = Entity();
			}

		}

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
				ImGui::Separator();
#ifndef DIST	//todo flesh out scene loading and saving routine etc. this basic implementation will allow me to save the current hard coded test scene and reload it again.
				if (ImGui::MenuItem("Save Scene"))
				{
					std::string filename = FileDialog::SaveFile("lvl");
					if (!filename.empty())
					{
						SaveScene(filename);
					}
				}
				ImGui::Separator();
#endif
				if (ImGui::MenuItem("Load Scene"))
				{
					std::string filename = FileDialog::OpenFile("lvl");
					if (!filename.empty())
					{
						LoadScene(filename);
					}
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("RunScene"))
			{
				if (ImGui::MenuItem("Start Scene")) m_ActiveScene->StartScene();
				ImGui::Separator();
				if (ImGui::MenuItem("Stop Scene")) m_ActiveScene->StopScene();
				ImGui::Separator();
				if (ImGui::MenuItem("Pause Scene")) m_ActiveScene->SetPaused(true);
				ImGui::Separator();
				if (ImGui::MenuItem("Unpause Scene")) m_ActiveScene->SetPaused(false);
				ImGui::Separator();
				if (ImGui::MenuItem("Reset Scene"))
				{
					m_ActiveScene->StopScene();
					m_ActiveScene = m_ResetScene;
					m_ResetScene = Scene::Copy(m_ActiveScene);
				}
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
			Application::Get().GetRenderer().GetCamera()->SetViewportSize((uint32_t)viewportpanelsize.x, (uint32_t)viewportpanelsize.y, false);
		}
		auto viewportminregion = ImGui::GetWindowContentRegionMin();
		auto viewportmaxregion = ImGui::GetWindowContentRegionMax();
		auto viewportoffset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = { viewportminregion.x + viewportoffset.x,viewportminregion.y + viewportoffset.y };
		m_ViewportBounds[1] = { viewportmaxregion.x + viewportoffset.x,viewportmaxregion.y + viewportoffset.y };
		m_ViewportHovered = ImGui::IsWindowHovered();
		m_ViewportFocused = ImGui::IsWindowFocused();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportHovered);
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
		m_ResetScene = Scene::Copy(m_ActiveScene);
#if 0
		//temp code for testing
		{
			Entity entity = m_ActiveScene->CreateEntity("Blue Rectangle");
			auto& transform = entity.GetComponent<TransformComponent>();
			transform.Translation.x = -100.0;
			transform.Translation.y = -100.0;
			transform.Translation.z = 0.0;
			transform.Rotation.x = 0.0;
			transform.Rotation.y = 0.0;
			transform.Rotation.z = 0.0;
			transform.Scale.x = 400.0;
			transform.Scale.y = 50.0;
			transform.Scale.z = 1.0;
			auto& src = entity.AddComponent<SpriteRendererComponent>(glm::vec4(0.0, 1.0, 1.0, 1.0));
			src.TilingFactor = 1;
			src.Tex = nullptr;
			auto& bc2d = entity.AddComponent<BoxCollider2DComponent>();
			auto& rb2d = entity.AddComponent<RigidBody2DComponent>();
			rb2d.Type = RigidBody2DComponent::BodyType::Static;
		}
		{
			Entity entity = m_ActiveScene->CreateEntity("Textured square");
			auto& transform = entity.GetComponent<TransformComponent>();
			transform.Translation.x = 0.0;
			transform.Translation.y = 200.0;
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
			auto& bc2d = entity.AddComponent<BoxCollider2DComponent>();
			auto& rb2d = entity.AddComponent<RigidBody2DComponent>();
			rb2d.Type = RigidBody2DComponent::BodyType::Dynamic;
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
#endif
	}
	void GameLayer::SaveScene(const std::string& filename)
	{
		if (!m_ActiveScene) return;
		SceneSerialiser serialiser(m_ActiveScene);
		serialiser.Serialise(filename);
	}
	void GameLayer::LoadScene(const std::string& filename)
	{
		std::shared_ptr<Scene> loadedscene = std::make_shared<Scene>();
		loadedscene->SetGameLayer(this);
		loadedscene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		SceneSerialiser deserialiser(loadedscene);
		ASSERT(deserialiser.Deserialise(filename), "failed to deserialise scene");
		m_ActiveScene = loadedscene;
#if done
		{
			//floor
			auto entity = m_ActiveScene->GetEntityByUUID(10729042005327288283);
			auto rb2d = entity.AddComponent<RigidBody2DComponent>();
			rb2d.FixedRotation = false;
			rb2d.Type = RigidBody2DComponent::BodyType::Static;
		}
		{
			//left side
			auto entity = m_ActiveScene->GetEntityByUUID(15550001477896860377);
			auto rb2d = entity.AddComponent<RigidBody2DComponent>();
			rb2d.FixedRotation = false;
			rb2d.Type = RigidBody2DComponent::BodyType::Static;
		}
		{
			//Right side
			auto entity = m_ActiveScene->GetEntityByUUID(11470402996329204725);
			auto rb2d = entity.AddComponent<RigidBody2DComponent>();
			rb2d.FixedRotation = false;
			rb2d.Type = RigidBody2DComponent::BodyType::Static;
		}
		{
			//ledge 1
			auto entity = m_ActiveScene->GetEntityByUUID(16071090857428972422);
			auto rb2d = entity.AddComponent<RigidBody2DComponent>();
			rb2d.FixedRotation = false;
			rb2d.Type = RigidBody2DComponent::BodyType::Static;
		}
		{
			//ledge 2
			auto entity = m_ActiveScene->GetEntityByUUID(1322686980937890548);
			auto rb2d = entity.AddComponent<RigidBody2DComponent>();
			rb2d.FixedRotation = false;
			rb2d.Type = RigidBody2DComponent::BodyType::Static;
		}
		{
			//clonemming
			auto& entity = m_ActiveScene->GetEntityByUUID(14113292102905541749);
			entity.AddComponent<ClonemmingComponent>();
			auto& sc = entity.AddComponent<ScriptComponent>();
			sc.ClassName = "Clonemmings.Clonemming";
		}
		{
			auto& entity = m_ActiveScene->GetEntityByUUID(14853091333380765479);
			entity.AddComponent<ClonemmingStartComponent>();
			auto& sc = entity.AddComponent<ScriptComponent>();
			sc.ClassName = "Clonemmings.ClonemmingSpawnPoint";
		}
		{
			auto& entity = m_ActiveScene->GetEntityByUUID(9136904962338667385);
			entity.AddComponent<ClonemmingExitComponent>();
			auto& sc = entity.AddComponent<ScriptComponent>();
			sc.ClassName = "Clonemmings.ClonemmingExitPoint";
		}

#endif
		m_ResetScene = Scene::Copy(m_ActiveScene);
	}
	void GameLayer::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseButtonPressedEvent>([this](auto&&...args)->decltype(auto) {return this->GameLayer::OnMouseButtonPressed(std::forward<decltype(args)>(args)...); });
	}
	bool GameLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == MouseButton::Leftbutton)
		{
			if (m_ViewportHovered && !Input::IsKeyPressed(Key::Leftalt))
			{
				m_CurrentSelectedEntity = m_HoveredEntity;
			}
		}
		return false;
	}
}