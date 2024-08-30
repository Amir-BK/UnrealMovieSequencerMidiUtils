// Copyright Epic Games, Inc. All Rights Reserved.

#include "MusicEditorEditorMode.h"
#include "MusicEditorEditorModeToolkit.h"
#include "EdModeInteractiveToolsContext.h"
#include "InteractiveToolManager.h"
#include "MusicEditorEditorModeCommands.h"


//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
// AddYourTool Step 1 - include the header file for your Tools here
//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
#include "Tools/MusicEditorSimpleTool.h"
#include "Tools/MusicEditorInteractiveTool.h"

// step 2: register a ToolBuilder in FMusicEditorEditorMode::Enter() below


#define LOCTEXT_NAMESPACE "MusicEditorEditorMode"

const FEditorModeID UMusicEditorEditorMode::EM_MusicEditorEditorModeId = TEXT("EM_MusicEditorEditorMode");

FString UMusicEditorEditorMode::SimpleToolName = TEXT("MusicEditor_ActorInfoTool");
FString UMusicEditorEditorMode::InteractiveToolName = TEXT("MusicEditor_MeasureDistanceTool");


UMusicEditorEditorMode::UMusicEditorEditorMode()
{
	FModuleManager::Get().LoadModule("EditorStyle");

	// appearance and icon in the editing mode ribbon can be customized here
	Info = FEditorModeInfo(UMusicEditorEditorMode::EM_MusicEditorEditorModeId,
		LOCTEXT("ModeName", "MusicEditor"),
		FSlateIcon(),
		true);
}


UMusicEditorEditorMode::~UMusicEditorEditorMode()
{
}


void UMusicEditorEditorMode::ActorSelectionChangeNotify()
{
}

void UMusicEditorEditorMode::Enter()
{
	UEdMode::Enter();

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// AddYourTool Step 2 - register the ToolBuilders for your Tools here.
	// The string name you pass to the ToolManager is used to select/activate your ToolBuilder later.
	//////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////// 
	const FMusicEditorEditorModeCommands& SampleToolCommands = FMusicEditorEditorModeCommands::Get();

	RegisterTool(SampleToolCommands.SimpleTool, SimpleToolName, NewObject<UMusicEditorSimpleToolBuilder>(this));
	RegisterTool(SampleToolCommands.InteractiveTool, InteractiveToolName, NewObject<UMusicEditorInteractiveToolBuilder>(this));

	// active tool type is not relevant here, we just set to default
	GetToolManager()->SelectActiveToolType(EToolSide::Left, SimpleToolName);
}

void UMusicEditorEditorMode::CreateToolkit()
{
	Toolkit = MakeShareable(new FMusicEditorEditorModeToolkit);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> UMusicEditorEditorMode::GetModeCommands() const
{
	return FMusicEditorEditorModeCommands::Get().GetCommands();
}

#undef LOCTEXT_NAMESPACE
