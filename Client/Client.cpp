#include "clientpch.h"
#include "EntryPoint.h"
#include "EditorLayer.h"

namespace SY {

	class Hazelnut : public Application
	{
	public:
		Hazelnut(const ApplicationSpecification& spec)
			: Application(spec)
		{
			PushLayer(new EditorLayer());
		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		ApplicationSpecification spec;
		spec.Name = L"Hazelnut";
		spec.CommandLineArgs = args;

		return new Hazelnut(spec);
	}

}