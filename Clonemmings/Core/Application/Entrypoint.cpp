#include "Core/Log/Log.h"
#include "Core/Application/Application.h"


int main(int argc, char** argv)
{
	Clonemmings::Log::Init();
	TRACE("Logging initialised!");
	
	INFO("Creating Applcation object");
	Clonemmings::Application* app = nullptr;
	Clonemmings::CommandLineArguements args;
	args.Args = argv;
	args.Count = argc;
	app = new Clonemmings::Application("Clonemmings", args);
	INFO("Running Application");
	app->Run();
	INFO("Shuting down, deleting application");
	delete app;
	return 0;
}