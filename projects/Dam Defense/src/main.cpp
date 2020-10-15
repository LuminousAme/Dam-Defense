#include "Titan/Scene.h"
#include "Titan/Application.h"

using namespace Titan;

int main() {
	TTN_Application::Init("Dam Defense", 800, 800);

	while (!TTN_Application::GetIsClosing())
		TTN_Application::Update();

	return 0;
}