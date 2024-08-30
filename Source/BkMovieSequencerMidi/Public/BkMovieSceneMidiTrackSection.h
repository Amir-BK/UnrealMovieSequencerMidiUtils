
#pragma once

#include "CoreMinimal.h"
//#include "Channels/MovieSceneAudioTriggerChannel.h"
//#include "Channels/MovieSceneChannelEditorData.h"
//#include "Channels/MovieSceneCurveChannelCommon.h"
//#include "Channels/MovieSceneBoolChannel.h"
//#include "Channels/MovieSceneFloatChannel.h"
//#include "Channels/MovieSceneIntegerChannel.h"
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

#include "BkMovieSceneMidiTrackSection.generated.h"


class UUndawSequenceMovieSceneTrack;
//class UDAWSequencerData;



UCLASS()
class BKMOVIESEQUENCERMIDI_API UBkMovieSceneMidiTrackSection : public UMovieSceneSection//, public IMovieSceneEntityProvider //, public IMidiBroadcaster

{
	GENERATED_BODY()

protected:

#if WITH_EDITOR //The midi section is only used in the editor to mark frames in the sequencer for animation purposes

	//Creates marked frames for all bars in the song
	UFUNCTION(CallInEditor, Category = "Midi")
	void MarkBars();

	//Create marked frames on each selected subdivision within the selection range 
	UFUNCTION(CallInEditor, Category = "Midi")
	void MarkSubdivisionsInRange();

	//Create marked frames for each note in the selection range on the selected midi track
	UFUNCTION(CallInEditor, Category = "Midi")
	void MarkNotesInRange();

#endif // WITH_EDITOR


	UPROPERTY()
	UBkMovieSceneMidiTrackSection* This = nullptr;

	UPROPERTY(EditAnywhere, Category = "Midi", meta = (InvalidEnumValues = "Beat, None"))
	EMidiClockSubdivisionQuantization MusicSubdivision = EMidiClockSubdivisionQuantization::QuarterNote;




public:

	UPROPERTY()
	int TrackIndexInParentSession = INDEX_NONE;


	UPROPERTY()
	TObjectPtr<UMidiFile> Midi;

	UPROPERTY()
	TArray<int32> MarkedFrames;


	UPROPERTY()
	FSequencerMidiNotesTrack MidiData;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Midi")
	FLinearColor NoteColor = FLinearColor::Red;
#endif // WITH_EDITOR_ONLY_DATA





	
public:
	UBkMovieSceneMidiTrackSection(const FObjectInitializer& ObjInit);


	virtual TOptional<TRange<FFrameNumber> > GetAutoSizeRange() const override { return TRange<FFrameNumber>::Empty(); }
	virtual void TrimSection(FQualifiedFrameTime TrimTime, bool bTrimLeft, bool bDeleteKeys) override {}
	//virtual UMovieSceneSection* SplitSection(FQualifiedFrameTime SplitTime, bool bDeleteKeys) override { return nullptr; }
	//virtual TOptional<FFrameTime> GetOffsetTime() const override;
	virtual void MigrateFrameTimes(FFrameRate SourceRate, FFrameRate DestinationRate) override {  };

#if WITH_EDITOR
	FText GetSectionTitle() const;
#endif 

	
protected:

	friend class UBkMovieSceneMidiTrack;

	UPROPERTY()
	TObjectPtr<UMovieSceneTrack> ParentTrack;



};
