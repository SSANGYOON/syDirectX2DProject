#pragma once
#include "Application.h"

extern SY::Application* SY::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{

	auto app = SY::CreateApplication({ argc, argv });

	app->Run();

	delete app;
}
