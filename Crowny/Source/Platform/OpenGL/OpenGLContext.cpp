#include "cwpch.h"

#include "Platform/OpenGL/OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Crowny
{
	OpenGLContext::OpenGLContext(void* window) : m_Window(window)
	{

	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent((GLFWwindow*)m_Window);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

		CW_ENGINE_ASSERT(status, "Failed to initialize GLAD!");

		CW_ENGINE_INFO("OpenGL Info:");
		CW_ENGINE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
		CW_ENGINE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
		CW_ENGINE_INFO("  Version: {0}", glGetString(GL_VERSION));
		int maxTextureSize;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

		int maxTextureSlots;
		glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &maxTextureSlots);

		CW_ENGINE_INFO("    Max Slots: ", maxTextureSlots);
		CW_ENGINE_INFO("    Max Size: ", maxTextureSlots, "x", maxTextureSlots);
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers((GLFWwindow*)m_Window);
	}
}