
#include "BkMovieSequencerMidiEditorModule.h"

#include "ISequencerModule.h"
#include "BkMovieSequencerMidiTrackEditor.h"

#define LOCTEXT_NAMESPACE "BkMovieSequencerMidiEditor"

void FBkMovieSequencerMidiEditorModule::StartupModule()
{
	ISequencerModule& SequencerModule = FModuleManager::LoadModuleChecked<ISequencerModule>("Sequencer");
	MidiTrackEditorHandle = SequencerModule.RegisterTrackEditor(FOnCreateTrackEditor::CreateStatic(&FBkMovieSceneMidiTrackEditor::CreateTrackEditor));
}

void FBkMovieSequencerMidiEditorModule::ShutdownModule()
{

	ISequencerModule& SequencerModule = FModuleManager::LoadModuleChecked<ISequencerModule>("Sequencer");
	SequencerModule.UnRegisterTrackEditor(MidiTrackEditorHandle);

}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FBkMovieSequencerMidiEditorModule, BkMovieSequencerMidiEditor)