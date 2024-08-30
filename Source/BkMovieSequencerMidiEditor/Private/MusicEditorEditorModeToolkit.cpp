// Copyright Epic Games, Inc. All Rights Reserved.

#include "MusicEditorEditorModeToolkit.h"
#include "MusicEditorEditorMode.h"
#include "Engine/Selection.h"

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "EditorModeManager.h"

#define LOCTEXT_NAMESPACE "MusicEditorEditorModeToolkit"

FMusicEditorEditorModeToolkit::FMusicEditorEditorModeToolkit()
{
}

void FMusicEditorEditorModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode)
{
	FModeToolkit::Init(InitToolkitHost, InOwningMode);
}

void FMusicEditorEditorModeToolkit::GetToolPaletteNames(TArray<FName>& PaletteNames) const
{
	PaletteNames.Add(NAME_Default);
}


FName FMusicEditorEditorModeToolkit::GetToolkitFName() const
{
	return FName("MusicEditorEditorMode");
}

FText FMusicEditorEditorModeToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("DisplayName", "MusicEditorEditorMode Toolkit");
}

#undef LOCTEXT_NAMESPACE
