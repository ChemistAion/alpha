#include "_editor.hpp"

#include "engine/renderer.hpp"
#include "interface/interface.hpp"

//#include "gltf/parser.hpp"

std::shared_ptr<Engine::Renderer> renderer_;
std::shared_ptr<Engine::SwapChain> swapchain_;

std::unique_ptr<Editor::Interface> interface_;

constexpr decltype(auto) cEditorWindowTitle_Name = _T("Alpha::EditorWindow");

int APIENTRY WinMain
(
	_In_ HINSTANCE instance,
	_In_opt_ HINSTANCE previousInstance,
	_In_ LPSTR commandLine,
	_In_ int showCommand
)
{
	assert(instance == ::GetModuleHandle(NULL));

	UNREFERENCED_PARAMETER(previousInstance);
	UNREFERENCED_PARAMETER(commandLine);
	UNREFERENCED_PARAMETER(showCommand);

	HANDLE process = ::GetCurrentProcess();

	::setlocale(LC_ALL, ""); //TODO: result
	::CoInitializeEx(NULL, COINIT_MULTITHREADED); //TODO: result
	::SymInitialize(process, NULL, TRUE); //TODO: result

	////////////////////////////////////////////////////////////////////////////////

	//auto console = Alpha::Core::Console::GetInstance();

	auto logger = Alpha::Core::Logger::GetInstance();
	logger->RegisterSink(std::make_unique<Alpha::Core::SinkMSVC>());
	logger->RegisterSink(std::make_unique<Alpha::Core::SinkSTD>());

	auto path = std::filesystem::current_path();

	logger->Info("Current path: " + path.string());

	////////////////////////////////////////////////////////////////////////////////

	if constexpr (Alpha::cAlphaMode_Debug)
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

	logger->Info("Editor::Initialize... begin");
	auto window = Alpha::Core::Window::GetInstance();

	window->Create();

	auto handle = window->GetHandle();
	::SetWindowText(handle, cEditorWindowTitle_Name);

	renderer_ = std::make_shared<Engine::Renderer>();

	Engine::SwapChainDesc swapchain_desc;
	swapchain_desc.name_ = "PrimarySwapChain";
	swapchain_desc.window_ = window;

	swapchain_ = renderer_->CreateSwapChain(swapchain_desc);
	
	interface_ = std::make_unique<Editor::Interface>
	(
		window,
		renderer_,
		swapchain_desc.format_,
		DXGI_FORMAT_UNKNOWN
	);
	
	logger->Info("Editor::Initialize... end");

	////////////////////////////////////////////////////////////////////////////////

	logger->Info("Editor... begin");

	MSG msg = {};
	Core::Timer timer;
	double cycle_time_ = 0.0;
	std::uint64_t frame = 0;;
	std::uint64_t total = 0;;

	for (;;)
	{
//		//TODO: vectorize handles...
//		DWORD result = ::MsgWaitForMultipleObjectsEx
//		(
//			0,
//			NULL,
//			1,
//			QS_ALLINPUT,
//			MWMO_ALERTABLE | MWMO_INPUTAVAILABLE
//		);
//		
//		if (WAIT_OBJECT_0 == result || WAIT_FAILED == result)
		{
			while (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				[[likely]] if (WM_QUIT != msg.message)
				{
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
		
					continue;
				}
		
				goto Terminate;
			}
		}

		////////////////////////////////////////////////////////////////////////////////

		auto time_update = timer.Update();
		//	auto frame = context->GetContextFrame();

		++frame;
		++total;

		auto time_total = timer.GetTotalTime();

		if (time_total - cycle_time_ > 1.0)
		{
			auto fps = (float)(frame / (time_total - cycle_time_));
			cycle_time_ = time_total;
			frame = 0;
		
			logger->Trace
			(
				"FPS: " + std::to_string(fps) + " Frame: " + std::to_string(total)
			);
		}

		////////////////////////////////////////////////////////////////////////////////

		try
		{
			interface_->BeginFrame(swapchain_);
			interface_->RenderFrame();
			interface_->EndFrame(swapchain_);		
		}
		catch (...)
		{
			logger->Critical("[Main]: Unhandled exception");
		}
	};

	////////////////////////////////////////////////////////////////////////////////

	Terminate:
	{
		logger->Info("Editor... end");
		logger->Info("Editor::Terminate... begin");

		//TODO: check it - already done in render.dtor
		//render_->GetImmediateContext()->Flush(false);

		interface_.reset();

		swapchain_.reset();
		renderer_.reset();

		if constexpr (Engine::cEngineMode_Debug && !Engine::cEngineMode_PIX)
		{
			HMODULE library = ::LoadLibraryEx(L"dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

			if (library)
			{
				typedef HRESULT(WINAPI* LPDXGIGETDEBUGINTERFACE)(REFIID, void**);
				auto debug = reinterpret_cast<LPDXGIGETDEBUGINTERFACE>(reinterpret_cast<void*>(::GetProcAddress(library, "DXGIGetDebugInterface")));

				IDXGIDebug* debug_controller;
				if (SUCCEEDED(debug(IID_PPV_ARGS(&debug_controller))))
				{
					auto result = debug_controller->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
					debug_controller->Release();
				}
			}
		}

		logger->Info("Editor::Terminate... end");
	};

	////////////////////////////////////////////////////////////////////////////////

	::CoUninitialize();
	::SymCleanup(process); //TODO: result

	assert(WM_QUIT == msg.message);
	return static_cast<int>(msg.wParam);
}

