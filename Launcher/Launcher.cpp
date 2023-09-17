// Exclude things we don't need from the Windows headers
//#define _ITERATOR_DEBUG_LEVEL = 2
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
#include "Launcher.h"
#include "Graphics/Rendering/GraphicsEngine.h"
#include "CommonUtilities/InputHandler.h"
#include "Editor/Editor.h"
#include <Engine.h>
#include <ComponentTools/SceneManager.h>
#include "../Game/Project.h"
#include "Resource.h"
#include "ExceptionHandler.h"

void Logic()
{
	while (true)
	{
		Engine::GetInstance()->Update();
		Project::Update();
	}
}


void Run(HINSTANCE hInstance)
{
	MSG msg = { 0 };
	bool editorMode = true;
#ifdef _GAME
	editorMode = false;
#endif
	HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

	std::shared_ptr<Engine> main = std::make_shared<Engine>();
	std::shared_ptr<GraphicsEngine> graphicsEngine = main->GetGraphicsEngine();
	bool enableOutPutMessages = false;
#ifdef _DEBUG
	enableOutPutMessages = true;
#endif  //DEBUG


	int x = GetSystemMetrics(SM_CXSCREEN);
	int y = GetSystemMetrics(SM_CYSCREEN);

	const SIZE windowSize = { x, y };


	bool bShouldRun = graphicsEngine->Initialize(
		(GetSystemMetrics(SM_CXSCREEN) - windowSize.cx) / 2,
		(GetSystemMetrics(SM_CYSCREEN) - windowSize.cy) / 2,
		windowSize.cx,
		windowSize.cy,
		enableOutPutMessages,
		editorMode, hIcon
	);


	//#ifdef _DEBUG
	//	//debug console window
	//	AllocConsole();
	//	FILE* f;
	//	freopen_s(&f, "CONOUT$", "w", stdout);
	//#endif

	main->Init(editorMode);
	if (!graphicsEngine->Start()) bShouldRun = false;

	Project::Setup();
	std::unique_ptr<Editor> editor = nullptr;
	if (editorMode)
	{
		editor = std::make_unique<Editor>();
		editor->Setup();
		editor->LoadDefaultScene();
	}

	//std::thread aThread([&] { Logic(); });


	while (bShouldRun)
	{
		// REMEMBER!
		// The frame update for the game does NOT happen inside the PeekMessage loop.
		// This would cause the game to only update if there are messages and also run
		// the update several times per frame (once for each message).


		auto start = std::chrono::system_clock::now();

		Engine::GetInstance()->Update();
		Project::Update();

		graphicsEngine->BeginFrame();
		graphicsEngine->RenderFrame();
		graphicsEngine->EndFrame();


		if (editorMode)
		{
			editor->Update();
		}

		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			Input::UpdateEvents(msg.message, msg.wParam, msg.lParam);
			// Should update input here.

			if (msg.message == WM_QUIT)
			{
				bShouldRun = false;
				Engine::GetInstance()->ShutDown();
			}
		}
	}
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	{
#ifdef _GAME
		__try
		{
			Run(hInstance);
		}
		__except (ExceptionFilterFunction(GetExceptionInformation()))
		{

		}
#else
		Run(hInstance);
#endif

	}

	return 0;
}