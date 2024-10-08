// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "BkMovieSequencerMidi.Generated.h"

USTRUCT()
struct FSequencerMidiNote
{
	GENERATED_BODY()

	UPROPERTY()
	int32 NoteNumber = INDEX_NONE;

	UPROPERTY()
	float StartTick = 0;

	UPROPERTY()
	float EndTick = 0;
};

USTRUCT()
struct FSequencerMidiNotesTrack
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FSequencerMidiNote> Notes;

	UPROPERTY(EditAnywhere, Category = "Midi")
	FString Name = "This is empty";

	UPROPERTY()
	int32 TrackIndexInMidiFile = INDEX_NONE;

	UPROPERTY()
	int32 ChannelIndexInMidiFile = INDEX_NONE;

	UPROPERTY(EditAnywhere, Category = "Midi", meta = (FullyExpand = false, MaxPropertyDepth = 0, HideAlphaChannel))
	FLinearColor TrackColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, Category = "Midi")
	bool bIsVisible = true;
};


class FBkMovieSequencerMidiModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
