#include "_sandbox.hpp"

constexpr decltype(auto) cEditorWindowTitle_Name = _T("Alpha::Sandbox");

int APIENTRY
WinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE previousInstance, _In_ LPSTR commandLine, _In_ int showCommand)
{
	assert(instance == ::GetModuleHandle(NULL));

	UNREFERENCED_PARAMETER(previousInstance);
	UNREFERENCED_PARAMETER(commandLine);
	UNREFERENCED_PARAMETER(showCommand);

	auto logger = Core::Logger::GetInstance();
	logger->RegisterSink(std::make_unique<Alpha::Core::SinkMSVC>());
	logger->RegisterSink(std::make_unique<Alpha::Core::SinkSTD>());

	////////////////////////////////////////////////////////////////////////////////

	HANDLE process = ::GetCurrentProcess();

	::setlocale(LC_ALL, "");                      //TODO: result
	::CoInitializeEx(NULL, COINIT_MULTITHREADED); //TODO: result
	::SymInitialize(process, NULL, TRUE);         //TODO: result

	if constexpr (cAlphaMode_Debug)
	{
		int debugFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

		debugFlag |= _CRTDBG_ALLOC_MEM_DF;
		debugFlag |= _CRTDBG_CHECK_ALWAYS_DF;
		debugFlag |= _CRTDBG_LEAK_CHECK_DF;

		_CrtSetDbgFlag(debugFlag);

		//auto mode = _CRTDBG_MODE_DEBUG | _CRTDBG_MODE_FILE;
		auto mode = _CRTDBG_MODE_DEBUG;

		_CrtSetReportMode(_CRT_WARN, mode);
		_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);

		_CrtSetReportMode(_CRT_ERROR, mode);
		_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);

		_CrtSetReportMode(_CRT_ASSERT, mode);
		_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
	}

	////////////////////////////////////////////////////////////////////////////////

	logger->Info("Sandbox::Initialize... begin");
	logger->Info("Current path: " + std::filesystem::current_path().string());

	Core::Config::Config config("config.json");

	//	if (config.IsEnabled("Console"))
	//	{
	//		auto console = Alpha::Core::Console::GetInstance();
	//	}

	auto window = Alpha::Core::Window::GetInstance();
	window->Create();
	
	auto handle = window->GetHandle();
	::SetWindowText(handle, cEditorWindowTitle_Name);

	auto registry = ECS::Registry::GetInstance();
	MSG msg = {};

	Core::Timer timer;

	try
	{
		const auto workers = config.GetValue<std::int64_t>("Workers");
		workers.second ? registry->Mount<Root::System::Worker>(workers.first) : registry->Mount<Root::System::Worker>();		

		//TODO: on Event::Create
		for (const auto& volume : config.GetNamesList("Volumes"))
		{
			registry->Emplace<Root::Component::Volume>(registry->Create(), volume);
		}
		
		const auto update = config.GetValue<double>("StorageUpdateRate");
		update.second ? registry->Mount<Root::System::Storage>(update.first) : registry->Mount<Root::System::Storage>();

		registry->Mount<Root::System::Engine>();
		registry->Emplace<Root::Component::Editor>(registry->Create());

		////////////////////////////////////////////////////////////////////////////////

		registry->Process(Root::Event::Create{});
	}
	catch (...)
	{
		logger->Critical("Sandbox::Init Unhandled exception");
		goto Terminate;
	}

	logger->Info("Sandbox::Initialize... end");

	////////////////////////////////////////////////////////////////////////////////

	logger->Info("Sandbox... begin");

	for (;;)
	{
		try
		{
			timer.Update();
			registry->Process(Root::Event::Tick(timer.GetTotalTime(), timer.GetUpdateTime()));

			//TODO: vectorize handles...
			DWORD result = ::MsgWaitForMultipleObjectsEx(0, NULL, 1, QS_ALLINPUT, MWMO_ALERTABLE | MWMO_INPUTAVAILABLE);
			if (WAIT_OBJECT_0 == result || WAIT_FAILED == result)
			{
				while (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
				{
					if (WM_QUIT != msg.message) [[likely]]
					{
						::TranslateMessage(&msg);
						::DispatchMessage(&msg);

						continue;
					}

					goto Terminate;
				}
			}
					
			registry->Process(Root::Event::Process{});

			registry->Process(Root::Event::Frame::Begin{});
			registry->Process(Root::Event::Frame::Render{});
			registry->Process(Root::Event::Frame::End{});

			registry->Process(Root::Event::Tock{});
		}
		catch (...)
		{
			logger->Critical("Sandbox::Main Unhandled exception");
			break;
		}
	};

	////////////////////////////////////////////////////////////////////////////////

Terminate:
	logger->Info("Sandbox... end");

	logger->Info("Sandbox::Terminate... begin");

	try
	{
		registry->Process(Root::Event::Terminate{});
	}
	catch (...)
	{
		logger->Critical("Sandbox::Main::Terminate Unhandled exception");
		goto Terminate;
	}

	::CoUninitialize();
	::SymCleanup(process); //TODO: result

	logger->Info("Sandbox::Terminate... end");

	assert(WM_QUIT == msg.message);
	return static_cast<int>(msg.wParam);
}
