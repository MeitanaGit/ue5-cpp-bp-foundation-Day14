// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO; // これを冒頭に追加

public class SampleProject : ModuleRules
{
	public SampleProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput", 
			"Niagara",
			"Paper2D"
		});


        // --- サブフォルダをすべて自動でインクルードパスに含める ---
        // ModuleDirectory（Source/Project)を起点に、全サブディレクトリを取得
        string[] AllSubDirectories = Directory.GetDirectories(ModuleDirectory, "*", SearchOption.AllDirectories);
		foreach (string Dir in AllSubDirectories)
		{
			PublicIncludePaths.Add(Dir);
		}
		// ルートディレクトリ自体も追加
		PublicIncludePaths.Add(ModuleDirectory);
		// -------------------------------------------------------------		
		
		
		PrivateDependencyModuleNames.AddRange(new string[] {  });


		
		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
