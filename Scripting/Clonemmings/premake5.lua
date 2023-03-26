project "Clonemmings"
kind "SharedLib"
language "C#"
dotnetframework "4.7.2"
targetdir ("%{wks.location}/Assets/Scripts")
objdir ("%{wks.location}/Assets/Scripts")

files
{
	"Source/**.cs",
	"Properties/**.cs"
}
links
{
	"Script-Core"
}

filter "configurations:Debug"
	optimize "Off"
	symbols "Default"

filter "configurations:Release"
	optimize "On"
	symbols "Default"

filter "configurations:Dist"
	optimize "Full"
	symbols "Off"