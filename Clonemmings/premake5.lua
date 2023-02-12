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
		".",
	}


	links
	{
		"../Dependencies/GLFW/bin/".. outputdir.."/GLFW/GLFW.lib",
		"../Dependencies/Glad/bin/".. outputdir.."/Glad/GLAD.lib",
		"../Dependencies/Box2D/bin/".. outputdir.."/Box2D/Box2D.lib",
		"../Dependencies/yaml-cpp/bin/".. outputdir.."/yaml-cpp/yaml-cpp.lib",
		"../Dependencies/ImGui/bin/".. outputdir.."/ImGui/ImGui.lib",
		"opengl32.lib"
	}
	defines 
	{
		"GLFW_INCLUDE_NONE",
		"_CRT_SECURE_NO_WARNINGS",
		"YAML_CPP_STATIC_DEFINE"
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
