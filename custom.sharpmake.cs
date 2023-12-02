using Sharpmake;
using System;
using System.IO;

// https://vector-of-bool.github.io/2018/09/16/layout-survey.html
// https://docs.microsoft.com/bs-cyrl-ba/cpp/build/profile-guided-optimizations?view=vs-2017

namespace Mockup
{
	////////////////////////////////////////////////////////////////////////////////

	[Fragment, Flags]
	public enum CustomOptimization
	{
		Debug = 0x01,
		Profile = 0x02,
		Release = 0x04,
		Final = 0x80
	}
	class CustomTarget : Sharpmake.ITarget
	{
		public Platform Platform = Platform.win64;
		public BuildSystem BuildSystem = BuildSystem.MSBuild;
		public DevEnv DevEnv = DevEnv.vs2019;
		public OutputType OutputType = OutputType.Lib;
		public DotNetFramework Framework = DotNetFramework.v4_7_2;
		public Blob Blob = Blob.NoBlob;

		public CustomOptimization Optimization;

		public CustomTarget()
        {
        }
		public CustomTarget(CustomOptimization optimization)
        {
			Optimization = optimization;
		}

		public override string Name
        {
			get { return Optimization.ToString(); }
		}		
}

	////////////////////////////////////////////////////////////////////////////////

	class CustomProject : Sharpmake.Project
	{
		public CustomProject() : base(typeof(CustomTarget))
		{
			RootPath = @"[project.SharpmakeCsPath]\.build\[project.Name]";

			SourceFilesExcludeRegex.Add(@"\\___+");

			CustomTarget target = new CustomTarget(CustomOptimization.Debug | CustomOptimization.Final | CustomOptimization.Profile | CustomOptimization.Release);
			AddTargets(target);

		}
	
		// [Configure]
		// void Configure(Configuration conf, Target target)
		// {
		// ConfigureOptimization(conf, target);
		// ConfigureDefines(conf, target);
		// }

		// public virtual void ConfigureOptimization(Configuration conf, Target target)
		// {
		// }
		// public virtual void ConfigureDefines(Configuration conf, Target target)
		// {
		// }

		[Configure()]
		public virtual void Configure(Configuration conf, CustomTarget target)
		{
			conf.ProjectFileName = "[project.Name]_[target.DevEnv]_[target.Platform]";
			conf.ProjectPath = @"[project.RootPath]";

			conf.PrecompHeader = @"_[project.Name].hpp";
			conf.PrecompSource = @"_[project.Name].cpp";
			conf.PrecompSourceExcludeFolders.Add(@"[project.SharpmakeCsPath]\external");
			conf.PrecompSourceExcludeFolders.Add(@"[project.SharpmakeCsPath]\tools");

			conf.Options.Add(Options.Vc.General.WarningLevel.Level3);
			conf.Options.Add(Options.Vc.General.WindowsTargetPlatformVersion.v10_0_19041_0);
			conf.Options.Add(Options.Vc.General.WindowsTargetPlatformVersion.Latest);
			conf.Options.Add(Options.Vc.General.PlatformToolset.v142);

			conf.Options.Add(Options.Vc.General.DebugInformation.ProgramDatabase);

			conf.Options.Add(Options.Vc.Compiler.CppLanguageStandard.Latest);
			conf.Options.Add(Options.Vc.Compiler.Exceptions.Enable);
			conf.Options.Add(Options.Vc.Compiler.RTTI.Enable);
			conf.Options.Add(Options.Vc.Compiler.FunctionLevelLinking.Disable);
			conf.Options.Add(Options.Vc.Compiler.RuntimeLibrary.MultiThreadedDLL);
			conf.Options.Add(Options.Vc.Compiler.Optimization.Disable);
			conf.Options.Add(Options.Vc.Compiler.Inline.Disable);
			conf.Options.Add(Options.Vc.Compiler.EnhancedInstructionSet.AdvancedVectorExtensions2);
			conf.Options.Add(Options.Vc.Compiler.SupportJustMyCode.Yes);

			conf.Options.Add(Options.Vc.Linker.SubSystem.Windows);

			conf.AdditionalCompilerOptions.Add(@"/bigobj");

			switch (target.Optimization)
			{
				case CustomOptimization.Debug:
					{
						conf.Options.Add(Options.Vc.Compiler.RuntimeLibrary.MultiThreadedDebug);
						conf.Options.Add(Options.Vc.Compiler.FavorSizeOrSpeed.Neither);
						conf.Options.Add(Options.Vc.Compiler.Optimization.Disable);

						//conf.Defines.Add("DEBUG");
						break;
					}
				case CustomOptimization.Profile:			
				case CustomOptimization.Release:
				case CustomOptimization.Final:
					{
						conf.Options.Add(Options.Vc.Compiler.RuntimeLibrary.MultiThreaded);
						conf.Options.Add(Options.Vc.Compiler.FavorSizeOrSpeed.FastCode);
						conf.Options.Add(Options.Vc.Compiler.Optimization.FullOptimization);
						conf.Options.Add(Options.Vc.Compiler.Inline.AnySuitable);

						// conf.Options.Add(Options.Vc.General.WholeProgramOptimization.LinkTime);
						break;
					}
			}

			conf.IncludePrivatePaths.Add(@"[project.SourceRootPath]");
			conf.IncludePrivatePaths.Add(@"[project.SharpmakeCsPath]\external");
			conf.IncludePrivatePaths.Add(@"[project.SharpmakeCsPath]\source");
			
			// conf.PrivateIncludePaths.Add();
			// conf.LibraryPaths.Add();
			// conf.LibraryFiles.Add();

			conf.VcxprojUserFile = new Project.Configuration.VcxprojUserFileSettings();
		}
	}

	////////////////////////////////////////////////////////////////////////////////

	class CustomLibrary : CustomProject
	{
		public CustomLibrary()
		{
			RootPath = @"[project.SharpmakeCsPath]\.build\[project.Name]";
		}

		public override void Configure(Configuration conf, CustomTarget target)
		{
			base.Configure(conf, target);
			conf.Output = Configuration.OutputType.Lib;
		}
	}

	////////////////////////////////////////////////////////////////////////////////

	class CustomProgram : CustomProject
	{
		public CustomProgram()
		{
			RootPath = @"[project.SharpmakeCsPath]\.build\[project.Name]";
		}
	}

	////////////////////////////////////////////////////////////////////////////////

	class CustomSolution : Sharpmake.Solution
	{
		public CustomSolution() : base(typeof(CustomTarget))
	    {
			CustomTarget target = new CustomTarget(CustomOptimization.Debug | CustomOptimization.Final | CustomOptimization.Profile | CustomOptimization.Release);
			AddTargets(target);
		}
	
		[Configure()]
		public virtual void Configure(Configuration conf, CustomTarget target)
	    {
			conf.SolutionFileName = "[solution.Name]_[target.DevEnv]_[target.Platform]";
			conf.SolutionPath = @"[solution.SharpmakeCsPath]\.build\";
		}
	}

	////////////////////////////////////////////////////////////////////////////////
}