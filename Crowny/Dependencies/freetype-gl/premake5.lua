project "freetype-gl"
    kind "StaticLib"
    language "C"

	systemversion "latest"
	staticruntime "On"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
    files
    {
        "texture-atlas.c",
        "texture-atlas.h",
        "texture-font.c",
		"texture-font.h",
		"vec234.h",
		"vector.c",
		"vector.h",
		"freetype-gl.h",
		"platform.c",
		"platform.h",
		"utf8-utils.h",
		"opengl.h",
		"utf8-utils.c",
		"distance-field.c",
		"distance-field.h",
		"edtaa3func.c",
		"edtaa3func.h"
    }

	includedirs
	{
		"../freetype2/include",
	}
	
    defines
    {
		"_CRT_NONSTDC_NO_DEPRECATE",
        "_CRT_SECURE_NO_WARNINGS"
    }
    
   filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Emscripten"
		runtime "Release"
		optimize "on"
