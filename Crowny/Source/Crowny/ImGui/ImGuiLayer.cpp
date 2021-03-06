#include "cwpch.h"  
  
#include "Crowny/ImGui/ImGuiLayer.h"  
#include "Crowny/Application/Application.h"  
#include "Crowny/Input/Input.h"  
  
#include "Platform/Vulkan/VulkanRendererAPI.h"  
#include "Platform/Vulkan/VulkanRenderPass.h"  
#include "Crowny/Renderer/CommandBuffer.h"  
  
#include <imgui.h>  
#include <ImGuizmo.h>  
#include <backends/imgui_impl_glfw.h>  
//#include <backends/imgui_impl_opengl3.h>  
#include <backends/imgui_impl_vulkan.h>  
#include <vulkan/vulkan.hpp>  
#include <GLFW/glfw3.h>  
  
namespace Crowny  
{  
  
	ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer")  
	{  
  
	}  
  
	void ImGuiLayer::OnAttach()  
	{  
		VkDescriptorPoolSize pool_sizes[] =  
		{  
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },  
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },  
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },  
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },  
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },  
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },  
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },  
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },  
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },  
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },  
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }  
		};  
  
		VkDescriptorPoolCreateInfo poolCreateInfo {};  
		poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;  
		poolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;  
		poolCreateInfo.maxSets = 1000;  
		poolCreateInfo.poolSizeCount = std::size(pool_sizes);  
		poolCreateInfo.pPoolSizes = pool_sizes;  
  
		VkDescriptorPool imguiPool;  
		VkResult result = vkCreateDescriptorPool(gVulkanRendererAPI().GetPresentDevice()->GetLogicalDevice(), &poolCreateInfo, nullptr, &imguiPool);  
		CW_ENGINE_ASSERT(result == VK_SUCCESS);  
  
		IMGUI_CHECKVERSION();  
		ImGui::CreateContext();  
		Application& app = Application::Get();  
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());  
		ImGuiIO& io = ImGui::GetIO(); (void)io;  
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  
		ImGui_ImplGlfw_InitForVulkan(window, true);  
  
		ImGui_ImplVulkan_InitInfo init_info {};  
		init_info.Instance = gVulkanRendererAPI().GetInstance();  
		init_info.PhysicalDevice = gVulkanRendererAPI().GetPresentDevice()->GetPhysicalDevice();  
		init_info.Device = gVulkanRendererAPI().GetPresentDevice()->GetLogicalDevice();  
		uint32_t numQueues = gVulkanRendererAPI().GetPresentDevice()->GetNumQueues(GRAPHICS_QUEUE);  
		init_info.Queue = gVulkanRendererAPI().GetPresentDevice()->GetQueue(GRAPHICS_QUEUE, numQueues - 1)->GetHandle();  
		init_info.DescriptorPool = imguiPool;  
		init_info.MinImageCount = gVulkanRendererAPI().GetSwapChain()->GetColorSurfacesCount();  
		init_info.ImageCount = gVulkanRendererAPI().GetSwapChain()->GetColorSurfacesCount();  
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;  
		init_info.Allocator = gVulkanAllocator;  
  
		VulkanRenderPassDesc passDesc;  
        passDesc.Samples = 1;  
        passDesc.Offscreen = false;  
        passDesc.Color[0].Format = gVulkanRendererAPI().GetSurfaceFormat().ColorFormat;  
        passDesc.Color[0].Enabled = true;  
  
		VulkanRenderPass* renderPass = VulkanRenderPasses::Get().GetRenderPass(passDesc);  
		ImGui_ImplVulkan_Init(&init_info, renderPass->GetHandle());  
  
		ImGui::StyleColorsDark();  
		ImGuiStyle& style = ImGui::GetStyle();  
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)  
		{  
			style.WindowRounding = 0.0f;  
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;  
		}  
		io.FontDefault = io.Fonts->AddFontFromFileTTF("Resources/Fonts/Roboto-Regular.ttf", 18.0f);  
		  
		Ref<VulkanCmdBuffer> cmdBuffer = std::static_pointer_cast<VulkanCmdBuffer>(CommandBuffer::Create(GRAPHICS_QUEUE));  
		ImGui_ImplVulkan_CreateFontsTexture(cmdBuffer->GetBuffer()->GetHandle());  
		gVulkanRendererAPI().SubmitCommandBuffer(cmdBuffer);  
		cmdBuffer->GetBuffer()->CheckFenceStatus(true);  
		ImGui_ImplVulkan_DestroyFontUploadObjects();  
  
		//Application& app = Application::Get();  
		//GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());  
  
		//ImGui_ImplGlfw_InitForOpenGL(window, true);  
		//ImGui_ImplOpenGL3_Init("#version 410");  
  
		  
		style.WindowRounding = 0;  
		style.GrabRounding = style.FrameRounding = 0;  
		style.ScrollbarSize = 15;  
		style.TabRounding = 0;  
		style.ScrollbarRounding = 5.0f;  
		style.FrameBorderSize = 1.0f;  
		style.ItemSpacing.y = 6.5f;  
  
		style.Colors[ImGuiCol_Text] = { 0.73333335f, 0.73333335f, 0.73333335f, 1.00f };  
		style.Colors[ImGuiCol_TextDisabled] = { 0.34509805f, 0.34509805f, 0.34509805f, 1.00f };  
		style.Colors[ImGuiCol_WindowBg] = { 0.18823529f, 0.1960784f, 0.20000000f, 0.94f };  
		style.Colors[ImGuiCol_ChildBg] = { 0.23529413f, 0.24705884f, 0.25490198f, 0.00f };  
		style.Colors[ImGuiCol_PopupBg] = { 0.23529413f, 0.24705884f, 0.25490198f, 0.94f };  
		style.Colors[ImGuiCol_Border] = { 0.33333334f, 0.33333334f, 0.33333334f, 0.50f };  
		style.Colors[ImGuiCol_BorderShadow] = { 0.15686275f, 0.15686275f, 0.15686275f, 0.00f };  
		style.Colors[ImGuiCol_FrameBg] = { 0.16862746f, 0.16862746f, 0.16862746f, 0.54f };  
		style.Colors[ImGuiCol_FrameBgHovered] = { 0.453125f, 0.67578125f, 0.99609375f, 0.67f };  
		style.Colors[ImGuiCol_FrameBgActive] = { 0.47058827f, 0.47058827f, 0.47058827f, 0.67f };  
		style.Colors[ImGuiCol_TitleBg] = { 0.04f, 0.04f, 0.04f, 1.00f };  
		style.Colors[ImGuiCol_TitleBgCollapsed] = { 0.16f, 0.29f, 0.48f, 1.00f };  
		style.Colors[ImGuiCol_TitleBgActive] = { 0.00f, 0.00f, 0.00f, 0.51f };  
		style.Colors[ImGuiCol_MenuBarBg] = { 0.27058825f, 0.28627452f, 0.2901961f, 0.80f };  
		style.Colors[ImGuiCol_ScrollbarBg] = { 0.27058825f, 0.28627452f, 0.2901961f, 0.60f };  
		style.Colors[ImGuiCol_ScrollbarGrab] = { 1.0f, 1.0f, 1.0f, 0.51f };  
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = { 0.5f, 0.5f, 0.5f, 1.00f };  
		style.Colors[ImGuiCol_ScrollbarGrabActive] = { 0.340f, 0.344f, 0.348f, 0.91f };  
		style.Colors[ImGuiCol_CheckMark] = { 0.90f, 0.90f, 0.90f, 0.83f };  
		style.Colors[ImGuiCol_SliderGrab] = { 0.70f, 0.70f, 0.70f, 0.62f };  
		style.Colors[ImGuiCol_SliderGrabActive] = { 0.30f, 0.30f, 0.30f, 0.84f };  
		style.Colors[ImGuiCol_Button] = { 0.33333334f, 0.3529412f, 0.36078432f, 0.49f };  
		style.Colors[ImGuiCol_ButtonHovered] = { 0.21960786f, 0.30980393f, 0.41960788f, 1.00f };  
		style.Colors[ImGuiCol_ButtonActive] = { 0.13725491f, 0.19215688f, 0.2627451f, 1.00f };  
		style.Colors[ImGuiCol_Header] = { 0.33333334f, 0.3529412f, 0.36078432f, 0.53f };  
		style.Colors[ImGuiCol_HeaderHovered] = { 0.453125f, 0.67578125f, 0.99609375f, 0.67f };  
		style.Colors[ImGuiCol_HeaderActive] = { 0.47058827f, 0.47058827f, 0.47058827f, 0.67f };  
		style.Colors[ImGuiCol_Separator] = { 0.31640625f, 0.31640625f, 0.31640625f, 1.00f };  
		style.Colors[ImGuiCol_SeparatorHovered] = { 0.31640625f, 0.31640625f, 0.31640625f, 1.00f };  
		style.Colors[ImGuiCol_SeparatorActive] = { 0.31640625f, 0.31640625f, 0.31640625f, 1.00f };  
		style.Colors[ImGuiCol_ResizeGrip] = { 1.00f, 1.00f, 1.00f, 0.85f };  
		style.Colors[ImGuiCol_ResizeGripHovered] = { 1.00f, 1.00f, 1.00f, 0.60f };  
		style.Colors[ImGuiCol_ResizeGripActive] = { 1.00f, 1.00f, 1.00f, 0.90f };  
		style.Colors[ImGuiCol_PlotLines] = { 0.61f, 0.61f, 0.61f, 1.00f };  
		style.Colors[ImGuiCol_PlotLinesHovered] = { 1.00f, 0.43f, 0.35f, 1.00f };  
		style.Colors[ImGuiCol_PlotHistogram] = { 0.90f, 0.70f, 0.00f, 1.00f };  
		style.Colors[ImGuiCol_PlotHistogramHovered] = { 1.00f, 0.60f, 0.00f, 1.00f };  
		style.Colors[ImGuiCol_TextSelectedBg] = { 0.18431373f, 0.39607847f, 0.79215693f, 0.90f };  
  
		style.Colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };  
  
		// Headers  
		style.Colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };  
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };  
		style.Colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };  
		  
		// Buttons  
		style.Colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };  
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };  
		style.Colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };  
  
		// Frame BG  
		style.Colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };  
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };  
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };  
  
		// Tabs  
		style.Colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };  
		style.Colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };  
		style.Colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };  
		style.Colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };  
		style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };  
  
		// Title  
		style.Colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };  
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };  
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };  
	}  
  
	void ImGuiLayer::OnDetach()  
	{  
		//ImGui_ImplOpenGL3_Shutdown();  
		ImGui_ImplVulkan_Shutdown();  
		ImGui_ImplGlfw_Shutdown();  
		ImGui::DestroyContext();  
	}  
  
	void ImGuiLayer::OnEvent(Event& e)  
	{  
		if (m_BlockEvents) {  
			ImGuiIO& io = ImGui::GetIO();  
			e.Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;  
			e.Handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;  
		}  
	}  
  
	void ImGuiLayer::Begin()  
	{  
		//ImGui_ImplOpenGL3_NewFrame();  
		ImGui_ImplVulkan_NewFrame();  
		ImGui_ImplGlfw_NewFrame();  
		  
		ImGui::NewFrame();  
		ImGuizmo::BeginFrame();  
	}  
  
	void ImGuiLayer::End()  
	{  
		ImGuiIO& io = ImGui::GetIO();  
		Application& app = Application::Get();  
		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());  
  
		ImGui::Render();  
		  
//		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());  
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), gVulkanRendererAPI().GetMainCommandBuffer()->GetHandle());  
  
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)  
		{  
//			GLFWwindow* backup_current_context = glfwGetCurrentContext();  
			ImGui::UpdatePlatformWindows();  
			ImGui::RenderPlatformWindowsDefault();  
//			glfwMakeContextCurrent(backup_current_context);  
		}  
	}  
  
}  
