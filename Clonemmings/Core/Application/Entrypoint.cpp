#include "Core/Log/Log.h"

int main(int argc, char** argv)
{
	Clonemmings::Log::Init();
	TRACE("Logging initialised!");
	return 0;
}