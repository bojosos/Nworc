#include "cwpch.h"

#include "Crowny/Renderer/RendererAPI.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"

namespace Crowny
{
	// Hmm a bit hidden?
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::Vulkan;
	
	uint32_t RendererAPI::VertexCountToPrimitiveCount(DrawMode drawMode, uint32_t elementCount)
	{
		switch (drawMode)
		{
			case DrawMode::POINT_LIST:
				return elementCount;
			case DrawMode::LINE_LIST:
				return elementCount / 2;
			case DrawMode::LINE_STRIP:
				return elementCount - 1;
			case DrawMode::TRIANGLE_LIST:
				return elementCount / 3;
			case DrawMode::TRIANGLE_STRIP:
				return elementCount - 2;
			case DrawMode::TRIANGLE_FAN:
				return elementCount - 2;
		}

		return 0;
	}
}