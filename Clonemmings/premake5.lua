project "Clonemmings"
	kind "consoleapp"
	language "c++"
	cppdialect "c++17"
	staticruntime "off"

	targetdir ("../bin/".. outputdir .. "/%{prj.name}")
	objdir ("..bin-int/".. outputdir .. "/%{prj.name}")
	
	
	files
	{
		"**.h",
		"**.cpp",
		
	}

	includedirs
	{
		"../Dependencies/GLFW/include",
		"../Dependencies/glm",
		"../Dependencies/Glad/include/glad",
		"../Dependencies/Glad/include",
		"../Dependencies/stb_image",
		"../Dependencies/Box2D/include",
		"../Dependencies/entt",
		"../Dependencies/ImGui",
		"../Dependencies/spdlog/include",
		"../Dependencies/yaml-cpp/include",
	}


	links
	{
		"GLFW",
		"GLAD",
		"Box2D",
		"yaml-cpp",
		"opengl32.lib"
	}
	defines 
	{
		"GLFW_INCLUDE_NONE",
		"_CRT_SECURE_NO_WARNINGS"
	}
	filter "system:windows"
		systemversion "latest"
	
	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
		defines 
		{
			"DEBUG"
		}

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
	filter "configurations:Dist"
		runtime "Release"
		optimize "on"
