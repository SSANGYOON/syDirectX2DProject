#include "clientpch.h"
#include "Hazelnut.h"

int main(int argc, char** argv)
{
	auto app = SY::CreateApplication({ argc, argv });

	app->Run();

	delete app;
}
