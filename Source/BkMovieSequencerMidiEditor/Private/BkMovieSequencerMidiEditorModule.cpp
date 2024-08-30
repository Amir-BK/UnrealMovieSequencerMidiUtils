// Copyright Epic Games, Inc. All Rights Reserved.

#include "BkMovieSequencerMidiEditorModule.h"
#include "MusicEditorEditorModeCommands.h"
#include "ISequencerModule.h"
#include <BkMovieSequencerMidiTrackEditor.h>

#define LOCTEXT_NAMESPACE "BkMovieSequencerMidiEditor"

void FBkMovieSequencerMidiEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	//FMusicEditorEditorModeCommands::Register();

	ISequencerModule& sequencerModule = FModuleManager::LoadModuleChecked<ISequencerModule>("Sequencer");
	sequencerModule.RegisterTrackEditor(FOnCreateTrackEditor::CreateStatic(&FBkMovieSceneMidiTrackEditor::CreateTrackEditor));
}

void FBkMovieSequencerMidiEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	//FMusicEditorEditorModeCommands::Unregister();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FBkMovieSequencerMidiEditorModule, BkMovieSequencerMidiEditor)