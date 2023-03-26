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
		"%{wks.location}/Dependencies/GLFW/include",
		"%{wks.location}/Dependencies/glm",
		"%{wks.location}/Dependencies/Glad/include/glad",
		"%{wks.location}/Dependencies/Glad/include",
		"%{wks.location}/Dependencies/stb_image",
		"%{wks.location}/Dependencies/Box2D/include",
		"%{wks.location}/Dependencies/entt",
		"%{wks.location}/Dependencies/ImGui",
		"%{wks.location}/Dependencies/spdlog/include",
		"%{wks.location}/Dependencies/yaml-cpp/include",
		"%{wks.location}/Dependencies/mono/include",
		"%{wks.location}/Dependencies/filewatch",
		".",
	}


	links
	{
		"%{wks.location}/Dependencies/GLFW/bin/".. outputdir.."/GLFW/GLFW.lib",
		"%{wks.location}/Dependencies/Glad/bin/".. outputdir.."/Glad/GLAD.lib",
		"%{wks.location}/Dependencies/Box2D/bin/".. outputdir.."/Box2D/Box2D.lib",
		"%{wks.location}/Dependencies/yaml-cpp/bin/".. outputdir.."/yaml-cpp/yaml-cpp.lib",
		"%{wks.location}/Dependencies/ImGui/bin/".. outputdir.."/ImGui/ImGui.lib",
		"%{wks.location}/Dependencies/mono/Libs/%{cfg.buildcfg}/libmono-static-sgen.lib",
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
		 
		links
		{
			"Ws2_32.lib",
			"Winmm.lib",
			"Version.lib",
			"Bcrypt.lib"
		}

	
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
