#include "EditorLayer.h"


#include "Crowny/Renderer/Renderer2D.h"
#include "Crowny/SceneManagement/SceneManager.h"
#include "Crowny/Ecs/Components.h"
#include "Crowny/ImGui/ImGuiTextureEditor.h"

#include "Crowny/Common/FileSystem.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

namespace Crowny
{
	
	EditorLayer::EditorLayer()
		: Layer("EditorLayer")
	{

	}

	void EditorLayer::OnAttach()
	{
		//Ref<Shader> s = Shader::Create("Shaders/PBRShader.glsl"); // To test syntax
		FramebufferProperties fbProps;
		fbProps.Width = 1280; // human code please
		fbProps.Height = 720;
		m_Framebuffer = Framebuffer::Create(fbProps);

		m_MenuBar = new ImGuiMenuBar();

		ImGuiMenu* fileMenu = new ImGuiMenu("File");
		fileMenu->AddItem(new ImGuiMenuItem("Exit", [](auto& event) { Application::Get().Exit(); })); m_MenuBar->AddMenu(fileMenu);

		ImGuiMenu* viewMenu = new ImGuiMenu("View");

		ImGuiMenu* renderInfo = new ImGuiMenu("Rendering Info");
		m_GLInfoWindow = new OpenGLInformationWindow("OpenGL");
		renderInfo->AddItem(new ImGuiMenuItem("OpenGL", [&](auto& event) { m_GLInfoWindow->Show(); }));
		viewMenu->AddMenu(renderInfo);

		m_HierarchyWindow = new ImGuiHierarchyWindow("Hierarchy");
		viewMenu->AddItem(new ImGuiMenuItem("Hierarchy", [&](auto& event) { m_HierarchyWindow->Show(); }));

		m_ViewportWindow = new ImGuiViewportWindow("Viewport", m_Framebuffer, m_ViewportSize);
		viewMenu->AddItem(new ImGuiMenuItem("Viewport", [&](auto& event) { m_ViewportWindow->Show(); }));

		m_ImGuiWindows.push_back(new ImGuiTextureEditor("Texture Properties"));
		viewMenu->AddItem(new ImGuiMenuItem("Texture Properties", [&](auto& event) { m_ImGuiWindows.back()->Show(); }));

		m_MenuBar->AddMenu(viewMenu);

		SceneManager::AddScene(new Scene("Editor scene")); // To be loaded

		
		auto [res, value] = FileSystem::SaveFileDialog();
		if (res)
			CW_ENGINE_WARN(value);
	}

	void EditorLayer::OnDetach()
	{
		for (ImGuiWindow* win : m_ImGuiWindows) 
		{
			delete win;
		}
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		if (FramebufferProperties spec = m_Framebuffer->GetProperties(); m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			SceneManager::GetActiveScene()->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		m_Framebuffer->Bind();
		SceneManager::GetActiveScene()->OnUpdate(ts);
		m_Framebuffer->Unbind();
	}

	void EditorLayer::OnImGuiRender()
	{
		//static bool show = true;
		ImGui::ShowDemoWindow();
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
		
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Crowny Editor", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("Crowny Editor");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		m_MenuBar->Render();

		m_HierarchyWindow->Render();
		m_GLInfoWindow->Render();
		m_ViewportWindow->Render();

		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		
	}

}