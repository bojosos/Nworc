#include "cwpch.h"
#include "Crowny.h"
#include <iostream>

class Test : public Crowny::Application
{
public:
	Test(int argc, char** argv)
	{
		
	}

	void OnUpdate(Crowny::Timestep ts)
	{
	}

	~Test()
	{

	}
};

int main(int argc, char** argv)
{
	std::cout << "Is it working?" << std::endl;
	Crowny::Log::Init();

	auto app = new Test(argc, argv);
	app->Run();
	delete app;
}