// Copyright Epic Games, Inc. All Rights Reserved.

#include "MusicEditorEditorModeCommands.h"
#include "MusicEditorEditorMode.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "MusicEditorEditorModeCommands"

FMusicEditorEditorModeCommands::FMusicEditorEditorModeCommands()
	: TCommands<FMusicEditorEditorModeCommands>("MusicEditorEditorMode",
		NSLOCTEXT("MusicEditorEditorMode", "MusicEditorEditorModeCommands", "MusicEditor Editor Mode"),
		NAME_None,
		FEditorStyle::GetStyleSetName())
{
}

void FMusicEditorEditorModeCommands::RegisterCommands()
{
	TArray <TSharedPtr<FUICommandInfo>>& ToolCommands = Commands.FindOrAdd(NAME_Default);

	UI_COMMAND(SimpleTool, "Show Actor Info", "Opens message box with info about a clicked actor", EUserInterfaceActionType::Button, FInputChord());
	ToolCommands.Add(SimpleTool);

	UI_COMMAND(InteractiveTool, "Measure Distance", "Measures distance between 2 points (click to set origin, shift-click to set end point)", EUserInterfaceActionType::ToggleButton, FInputChord());
	ToolCommands.Add(InteractiveTool);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> FMusicEditorEditorModeCommands::GetCommands()
{
	return FMusicEditorEditorModeCommands::Get().Commands;
}

#undef LOCTEXT_NAMESPACE
