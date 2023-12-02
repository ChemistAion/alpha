using Sharpmake;
using System;
//using System.Data;
//using System.Diagnostics;

[module: Sharpmake.Include("custom.sharpmake.cs")]

namespace Mockup
{
	[Sharpmake.Generate]
	class google_test : CustomLibrary
	{
		public google_test()
		{
			SourceRootPath = @"[project.SharpmakeCsPath]\tools\googletest\googletest";
			SourceFilesExcludeRegex.Add(@"\\cmake");
			SourceFilesExcludeRegex.Add(@"\\samples");
			SourceFilesExcludeRegex.Add(@"\\scripts");			
			SourceFilesExcludeRegex.Add(@"\\test");
			SourceFilesExcludeRegex.Add(@"gtest-all.cc");
		}
	
		public override void Configure(Configuration conf, CustomTarget target)
		{
			base.Configure(conf, target);
			
			conf.Defines.Add("GTEST_ENABLE_CATCH_EXCEPTIONS_=1");
			conf.Defines.Add("gtest_build_tests=OFF");
			conf.Defines.Add("gtest_build_samples=OFF");
			
			conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\tools\googletest\googletest\include");
		}
	}
	
	[Sharpmake.Generate]
	class google_mock : CustomLibrary
	{
		public google_mock()
		{
			SourceRootPath = @"[project.SharpmakeCsPath]\tools\googletest\googlemock";
			SourceFilesExcludeRegex.Add(@"\\cmake");
			SourceFilesExcludeRegex.Add(@"\\scripts");
			SourceFilesExcludeRegex.Add(@"\\test");
		}
	
		public override void Configure(Configuration conf, CustomTarget target)
		{
			base.Configure(conf, target);
					
			conf.AddPrivateDependency<google_test>(target);
			
			conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\tools\googletest\googlemock\include");
		}
	}
	
	[Sharpmake.Generate]
	class google_benchmark : CustomLibrary
	{
		public google_benchmark()
		{
			SourceRootPath = @"[project.SharpmakeCsPath]\tools\benchmark";
			SourceFilesExcludeRegex.Add(@"\\bindings");
			SourceFilesExcludeRegex.Add(@"\\cmake");
			SourceFilesExcludeRegex.Add(@"\\conan");
			SourceFilesExcludeRegex.Add(@"\\test");		
			////SourceFilesExcludeRegex.Add(@"\\tools");
		}
	
		public override void Configure(Configuration conf, CustomTarget target)
		{
			base.Configure(conf, target);
			
			conf.AddPrivateDependency<google_test>(target);
			conf.AddPrivateDependency<google_mock>(target);
			conf.AddPrivateDependency<core>(target);
			
			conf.Defines.Add("BENCHMARK_ENABLE_EXCEPTIONS=1");
			conf.Defines.Add("BENCHMARK_USE_LIBCXX=0");
								
			conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\tools\benchmark\include");
		}
	}

    ////////////////////////////////////////////////////////////////////////////////

    [Sharpmake.Generate]
	class core : CustomLibrary
    {
		public core()
		{
			SourceRootPath = @"[project.SharpmakeCsPath]\source\core";
			
			//simdjson
			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\simdjson\src\simdjson.cpp");
		}
		
		public override void Configure(Configuration conf, CustomTarget target)
		{
			base.Configure(conf, target);
			
			//simdjson
			conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\external\simdjson\include");
			conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\external\simdjson\src");
			
			//cista
			conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\external\cista\include");

			//platform
			conf.LibraryFiles.Add("dbghelp.lib");
			conf.LibraryFiles.Add("shlwapi.lib");
		}
    }
	
	[Sharpmake.Generate]
	class ecs : CustomLibrary
    {
		public ecs()
		{
			SourceRootPath = @"[project.SharpmakeCsPath]\source\ecs";
		}
		
		public override void Configure(Configuration conf, CustomTarget target)
		{
			base.Configure(conf, target);
			
			conf.AddPrivateDependency<core>(target);
		}
    }
	
//	[Sharpmake.Generate]
//	class gltf : CustomLibrary
//	{
//		public gltf()
//		{
//			SourceRootPath = @"[project.SharpmakeCsPath]\source\gltf";
//			
//			//simdjson
//			//SourceFiles.Add(@"[project.SharpmakeCsPath]\external\simdjson\src\simdjson.cpp");
//			//SourceFiles.Add(@"[project.SharpmakeCsPath]\external\simdjson\include\simdjson.h");
//			
//			//cista
//			//SourceFiles.Add(@"[project.SharpmakeCsPath]\external\cista\include\cista\serialization.h");
//		}
//		
//		public override void Configure(Configuration conf, CustomTarget target)
//		{
//			base.Configure(conf, target);
//	
//			//conf.AddPrivateDependency<core>(target);
//			//conf.AddPublicDependency<core>(target);
//	
//			//simdjson
//			//conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\external\simdjson\include");
//			//conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\external\simdjson\src");
//			
//			//cista
//			//conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\external\cista\include");
//		}
//	}

//	[Sharpmake.Generate]
//	class mockup : CustomLibrary
//	{
//		public mockup()
//		{
//			SourceRootPath = @"[project.SharpmakeCsPath]\source\mockup";
//		}
//		
//		public override void Configure(Configuration conf, CustomTarget target)
//		{
//			base.Configure(conf, target);
//	
//			conf.AddPrivateDependency<core>(target);
//			//conf.AddPrivateDependency<gltf>(target);
//			conf.AddPrivateDependency<engine>(target);
//		}
//	}
		
	[Sharpmake.Generate]
	class engine : CustomLibrary
	{
		public engine()
		{
			SourceRootPath = @"[project.SharpmakeCsPath]\source\engine";
		}
	
		public override void Configure(Configuration conf, CustomTarget target)
		{
			base.Configure(conf, target);
	
			conf.AddPrivateDependency<core>(target);
			//conf.AddPublicDependency<core>(target);
	
			conf.LibraryPaths.Add(@"\external\xcompiler\build\release\lib");
	
			conf.TargetCopyFiles.Add(KitsRootPaths.GetRoot(KitsRootEnum.KitsRoot10) + @"bin\10.0.19041.0\x64\dxil.dll");
			conf.TargetCopyFiles.Add(@"\external\xcompiler\build\release\bin\dxcompiler.dll");
	
			conf.LibraryFiles.Add("dxgi.lib");
			conf.LibraryFiles.Add("d3d12.lib");
			conf.LibraryFiles.Add("dxguid.lib");
			conf.LibraryFiles.Add("dxcompiler.lib");
			
			conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\external\xheaders\include\directx");
		}
	}

	[Sharpmake.Generate]
	class root : CustomLibrary
	{
		public root()
		{
			SourceRootPath = @"[project.SharpmakeCsPath]\source\root";
			
			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imconfig.h");
			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imgui_internal.h");
			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imstb_rectpack.h");
			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imstb_textedit.h");
			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imstb_truetype.h");
			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imgui.h");
			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imgui.cpp");
			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imgui_draw.cpp");
			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imgui_widgets.cpp");
			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imgui_tables.cpp");
			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\backends\imgui_impl_win32.h");
			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\backends\imgui_impl_win32.cpp");
			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imgui_demo.cpp");
		}
		
		public override void Configure(Configuration conf, CustomTarget target)
		{
			base.Configure(conf, target);
	
			conf.AddPublicDependency<core>(target);
			conf.AddPrivateDependency<ecs>(target);
			conf.AddPrivateDependency<engine>(target);
			//conf.AddPrivateDependency<gltf>(target);
			//conf.AddPrivateDependency<mockup>(target);
			
			conf.IncludePrivatePaths.Add(@"[project.SharpmakeCsPath]\external\imgui");
		}
	}
	
	////////////////////////////////////////////////////////////////////////////////

	[Sharpmake.Generate]
	class sandbox : CustomProgram
	{
		public sandbox()
		{
			SourceRootPath = @"[project.SharpmakeCsPath]\source\sandbox";	
			ResourceFiles.Add(@"[project.SharpmakeCsPath]\source\core\platform\resources.rc");
		}
	
		public override void Configure(Configuration conf, CustomTarget target)
		{
			base.Configure(conf, target);
	
			conf.AddPrivateDependency<core>(target);
			conf.AddPrivateDependency<root>(target);		
			conf.AddPrivateDependency<engine>(target);
			//conf.AddPrivateDependency<gltf>(target);
			//conf.AddPrivateDependency<mockup>(target);
			
			conf.VcxprojUserFile.LocalDebuggerWorkingDirectory = @"[project.SharpmakeCsPath]\workdir";
		}
	}

	[Sharpmake.Generate]
	class tests : CustomProgram
	{
		public tests()
		{
			SourceRootPath = @"[project.SharpmakeCsPath]\workbench\tests";
		}
		
		public override void Configure(Configuration conf, CustomTarget target)
		{
			base.Configure(conf, target);
			
			conf.Options.Add(Options.Vc.Linker.SubSystem.Console);
			
			conf.AddPrivateDependency<core>(target);
			conf.AddPrivateDependency<ecs>(target);			
			//conf.AddPrivateDependency<root>(target);
			//conf.AddPrivateDependency<gltf>(target);
			//conf.AddPrivateDependency<mockup>(target);
			
			conf.AddPrivateDependency<google_test>(target);
			
			conf.VcxprojUserFile.LocalDebuggerWorkingDirectory = @"[project.SharpmakeCsPath]\workdir";
		}
	}
	
//	[Sharpmake.Generate]
//	class benchmarks : CustomProgram
//	{
//		public benchmarks()
//		{
//			SourceRootPath = @"[project.SharpmakeCsPath]\workbench\benchmarks";
//		}
//		
//		public override void Configure(Configuration conf, CustomTarget target)
//		{
//			base.Configure(conf, target);
//			
//			conf.Options.Add(Options.Vc.Linker.SubSystem.Console);
//			
//			conf.AddPrivateDependency<core>(target);
//			conf.AddPrivateDependency<google_benchmark>(target);
//			
//			conf.VcxprojUserFile.LocalDebuggerWorkingDirectory = @"[project.SharpmakeCsPath]\workdir";
//		}
//	}	
	
	////////////////////////////////////////////////////////////////////////////////

	[Sharpmake.Generate]
	class alpha : CustomSolution
    {
		public alpha()
		{
            ExtraItems["extras"] = new Strings
			{
                @"[solution.SharpmakeCsPath]\.clang-format",
            };
		}
			
		public override void Configure(Configuration conf, CustomTarget target)
        {
			base.Configure(conf, target);

			conf.AddProject<sandbox>(target);		
			conf.AddProject<tests>(target);
			//conf.AddProject<benchmarks>(target);
        }
	}

	public static class Bootstrap
	{
		[Sharpmake.Main]
		public static void SharpmakeMain(Sharpmake.Arguments arguments)
		{
			arguments.Generate<alpha>();
		}
	}
}