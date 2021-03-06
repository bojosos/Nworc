#include "cwpch.h"

#include "Crowny/Renderer/Font.h"
#include "Crowny/Common/VirtualFileSystem.h"

#include <freetype-gl.h>

namespace Crowny
{

	Font::Font(const std::string& name, const std::string& path, float size) : m_Name(name), m_Filepath(path), m_Size(size)
	{
		m_Atlas = ftgl::texture_atlas_new(512, 512, 1);
		auto [mem, memSize] = VirtualFileSystem::Get()->ReadFile(path);
		m_Font = ftgl::texture_font_new_from_memory(m_Atlas, m_Size, mem, memSize);

		for (uint8_t c = 0; c < 128; c++)
		{
			char f = (char)c;
			texture_font_load_glyph(m_Font, &f);
		}

		TextureParameters params = { TextureFormat::RGBA8, TextureFilter::LINEAR, TextureWrap::REPEAT };

		m_Texture = Texture2D::Create(512, 512, params);

		byte* data = new byte[512 * 512 * 4];
		memset(data, 0xff, 512 * 512 * 4);

		for (uint32_t i = 0; i < 512 * 512; i++)
		{
			data[i * 4 + 3] = m_Atlas->data[i];
		}

		m_Texture->SetData(data, TextureChannel::CHANNEL_RGBA);

		delete[] data;
		delete mem;
	}

	std::vector<Ref<Font>> FontManager::s_Fonts;

	void FontManager::Add(const Ref<Font>& font)
	{
		s_Fonts.push_back(font);
	}

	Ref<Font> FontManager::Get(const std::string& name)
	{
		for (Ref<Font>& font : s_Fonts)
		{
			if (font->GetName() == name)
			{
				return font;
			}
		}
		CW_ENGINE_ERROR("Unrecognized font, using fallback font {0}!", s_Fonts[0]->GetName());
		return s_Fonts[0];
	}

	Ref<Font> FontManager::Get(const std::string& name, float size)
	{
		for (Ref<Font>& font : s_Fonts)
		{
			if (font->GetName() == name)
			{
				if (font->GetSize() == size)
				{
					return font;
				}
				else
				{
					s_Fonts.push_back(CreateRef<Font>(name, font->GetFilepath(), size));
					return s_Fonts.back();
				}
			}
		}

		CW_ENGINE_ERROR("Unrecognized font, using fallback font {0}!", s_Fonts[0]->GetName());
		if (s_Fonts[0]->GetSize() == size)
		{
			return s_Fonts[0];
		}
		else
		{
			s_Fonts.push_back(CreateRef<Font>(name, DEFAULT_FONT_FILENAME, size));
			return s_Fonts.back();
		}

		return s_Fonts[0];
	}

	float Font::GetWidth(const std::string& fontName, const std::string& text)
	{
		return GetWidth(FontManager::Get(fontName), text);
	}

	float Font::GetWidth(const Ref<Font>& font, const std::string& text)
	{
		float width = 0.0f;

		for (int i = 0; i < text.size(); i++)
		{
			ftgl::texture_glyph_t* glyph = texture_font_get_glyph(font->GetFTGLFont(), &text[i]);
			if (i > 0)
			{
				float kerning = texture_glyph_get_kerning(glyph, &text[i - 1]);
				width += kerning;
			}
			width += glyph->advance_x;
		}

		return width;
	}

	float Font::GetHeight(const std::string& fontName, const std::string& text)
	{
		return GetHeight(FontManager::Get(fontName), text);
	}

	float Font::GetHeight(const Ref<Font>& font, const std::string& text)
	{
		float min = 0.0f;
		float max = 0.0f;
		for (int i = 0; i < text.size(); i++)
		{
			ftgl::texture_glyph_t* glyph = texture_font_get_glyph(font->GetFTGLFont(), &text[i]);
			float height = glyph->height;
			float offset = glyph->offset_y - height;
			if (offset < min)
				min = offset;
			if (height > max)
				max = height;
		}
		return abs(min) + abs(max);
	}

}