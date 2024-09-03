#pragma once

#include "CoreMinimal.h"
//#include "Channels/MovieSceneAudioTriggerChannel.h"
//#include "Channels/MovieSceneChannelEditorData.h"
//#include "Channels/MovieSceneCurveChannelCommon.h"
//#include "Channels/MovieSceneBoolChannel.h"
#include "Channels/MovieSceneFloatChannel.h"
#include "Channels/MovieSceneIntegerChannel.h"
//#include "Channels/MovieSceneStringChannel.h"
#include "HarmonixMidi/MidiFile.h"
//#include "MidiBroadcasters/MidiBroadcaster.h"
//#include "MidiBroadcasters/MidiBroadcasterPlayHead.h"
//#include "MidiObjects/MidiAsset.h"
#include "Sections/MovieSceneHookSection.h"
#include "MovieSceneTrack.h"
#include "BkMovieSequencerMidi.h"
//#include "M2SoundGraphData.h"
#include <HarmonixMidi/SongMaps.h>
#include "HarmonixMidi/Blueprint/MidiNote.h"
#include "HarmonixMetasound/DataTypes/MidiEventInfo.h"

#include "BkMovieSceneMidiTrackSection.generated.h"

class UUndawSequenceMovieSceneTrack;
//class UDAWSequencerData;

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnMidiNote, FMidiEventInfo, Note);

UCLASS()
class BKMOVIESEQUENCERMIDI_API UBkMovieSceneMidiTrackSection : public UMovieSceneSection//, public IMovieSceneEntityProvider //, public IMidiBroadcaster

{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Midi", meta = (AutoCreateRefTerm = "InDelegate", Keywords = "Event, Quantization, DAW"))
	void SubscribeToMidiNoteEventsOnTrackRow(FOnMidiNote InDelegate, int32 InRowIndex) {};

	/** Set the offset into the beginning of the audio clip */
	UFUNCTION(BlueprintCallable, Category = "Sequencer|Section")
	void SetStartOffset(FFrameNumber InStartOffset) { StartFrameOffset = InStartOffset; }

	/** Get the offset into the beginning of the audio clip */
	UFUNCTION(BlueprintPure, Category = "Sequencer|Section")
	FFrameNumber GetStartOffset() const { return StartFrameOffset; }

protected:

#if WITH_EDITOR //The midi section is only used in the editor to mark frames in the sequencer for animation purposes

	//Creates marked frames for all bars in the song
	UFUNCTION(CallInEditor, Category = "Midi")
	void MarkBars();

	//Create marked frames on each selected subdivision within the selection range
	UFUNCTION(CallInEditor, Category = "Midi")
	void MarkSubdivisionsInRange();

#endif // WITH_EDITOR

	UFUNCTION(CallInEditor, Category = "Midi")
	void RebuildNoteKeyFrames();

	virtual void ParseRawMidiEventsIntoNotesAndChannels(UMidiFile* InMidiFile);

	UPROPERTY(EditAnywhere, Category = "Midi", meta = (InvalidEnumValues = "Beat, None"))
	EMidiClockSubdivisionQuantization MusicSubdivision = EMidiClockSubdivisionQuantization::QuarterNote;

	//If true, only marks frames within the selection range, if the selection range is empty, marks frames in the entire section
	UPROPERTY(EditAnywhere, Category = "Midi")
	bool bMarkOnlyInSelectionRange = true;

public:

	//TODO remove, not needed
	UPROPERTY()
	int TrackIndexInParentSession = INDEX_NONE;

	UPROPERTY()
	int MaxNotePitch = 127;

	UPROPERTY()
	int MinNotePitch = 0;

	UPROPERTY()
	TObjectPtr<UMidiFile> Midi;

	UPROPERTY()
	TArray<int32> MarkedFrames;

	UPROPERTY()
	float SectionHeight = 150.0f;

	UPROPERTY()
	TArray<FMovieSceneIntegerChannel> MidiNoteChannels;

	//UPROPERTY(VisibleAnywhere, Category = "Midi")
	//TMap<int32, FSequencerMidiNotesTrack> MidiTracks;

	UPROPERTY(EditAnywhere, EditFixedSize, Category = "Midi", meta = (EditFixedSize, TitleProperty = "Name", NoResetToDefault))
	TArray<FSequencerMidiNotesTrack> MidiChannels;

public:
	UBkMovieSceneMidiTrackSection(const FObjectInitializer& ObjInit);

	virtual TOptional<TRange<FFrameNumber> > GetAutoSizeRange() const override { return TRange<FFrameNumber>::Empty(); }
	virtual void TrimSection(FQualifiedFrameTime TrimTime, bool bTrimLeft, bool bDeleteKeys) override;
	virtual UMovieSceneSection* SplitSection(FQualifiedFrameTime SplitTime, bool bDeleteKeys) override;
	virtual TOptional<FFrameTime> GetOffsetTime() const override;
	virtual void MigrateFrameTimes(FFrameRate SourceRate, FFrameRate DestinationRate) override;
	virtual EMovieSceneChannelProxyType CacheChannelProxy() override;

#if WITH_EDITOR
	FText GetSectionTitle() const;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:

	friend class UBkMovieSceneMidiTrack;

	UPROPERTY()
	TObjectPtr<UMovieSceneTrack> ParentTrack;

	/** The offset into the beginning of the midi clip */
	UPROPERTY(EditAnywhere, Category = "Midi")
	FFrameNumber StartFrameOffset;
};
