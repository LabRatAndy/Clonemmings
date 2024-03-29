workspace "Clonemmings"
	architecture "x64"
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
	startproject "Clonemmings"

include  "Clonemmings"
group "Dependencies"
	include  "Dependencies/Box2D"
	include  "Dependencies/Glad"
	include  "Dependencies/GLFW"
	include  "Dependencies/yaml-cpp"
	include  "Dependencies/ImGui"
group ""
