
#pragma once

#include "CoreMinimal.h"
#include "Channels/MovieSceneAudioTriggerChannel.h"
#include "Channels/MovieSceneChannelEditorData.h"
#include "Channels/MovieSceneCurveChannelCommon.h"
#include "Channels/MovieSceneBoolChannel.h"
#include "Channels/MovieSceneFloatChannel.h"
#include "Channels/MovieSceneIntegerChannel.h"
#include "Channels/MovieSceneStringChannel.h"
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

	//Creates marked frames for all bars in the song
	UFUNCTION(CallInEditor, Category = "Midi")
	void MarkBars();

	//Create marked frames on each selected subdivision within the selection range 
	UFUNCTION(CallInEditor, Category = "Midi")
	void MarkSubdivisionsInRange();

	//Create marked frames for each note in the selection range on the selected midi track
	UFUNCTION(CallInEditor, Category = "Midi")
	void MarkNotesInRange();

	UPROPERTY()
	UBkMovieSceneMidiTrackSection* This = nullptr;

	UPROPERTY(EditAnywhere, Category = "Midi", BlueprintReadWrite)
	float PlayRate = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Midi", meta = (InvalidEnumValues = "Beat, None"))
	EMidiClockSubdivisionQuantization MusicSubdivision = EMidiClockSubdivisionQuantization::QuarterNote;

	////when true, the section will only mark the frames that are within the selection range
	//UPROPERTY(EditAnywhere, Category = "Midi")
	//bool bMarkOnlyInSelection = false;


	double SectionLocalCurrentTime=0;


public:

	UPROPERTY()
	int TrackIndexInParentSession = INDEX_NONE;

	UPROPERTY()
	FMovieSceneFloatChannel SoundVolume;

	UPROPERTY()
	FMovieSceneFloatChannel PitchBend;

	UPROPERTY()
	TObjectPtr<UMidiFile> Midi;


	UPROPERTY()
	FSequencerMidiNotesTrack MidiData;
	

	//UPROPERTY()
	//FMovieSceneMidiTrackChannel MidiNotes;

	UPROPERTY(EditAnywhere, Category = "Midi")
	FString MidiFileName = "Midi Track Name";




	
public:
	UBkMovieSceneMidiTrackSection(const FObjectInitializer& ObjInit);


	virtual TOptional<TRange<FFrameNumber> > GetAutoSizeRange() const override { return TRange<FFrameNumber>::Empty(); }
	virtual void TrimSection(FQualifiedFrameTime TrimTime, bool bTrimLeft, bool bDeleteKeys) override {}
	//virtual UMovieSceneSection* SplitSection(FQualifiedFrameTime SplitTime, bool bDeleteKeys) override { return nullptr; }
	virtual TOptional<FFrameTime> GetOffsetTime() const override;
	virtual void MigrateFrameTimes(FFrameRate SourceRate, FFrameRate DestinationRate) override {  };

	//virtual EMovieSceneChannelProxyType CacheChannelProxy() override
	//{
	//	FMovieSceneChannelProxyData Channels;

	//	UBkMovieSceneMidiTrackSection* MutableThis = const_cast<UBkMovieSceneMidiTrackSection*>(this);
	//	UMovieScene* MovieScene = MutableThis->GetTypedOuter<UMovieScene>();

	//	FMovieSceneChannelMetaData MetaData;
	//	MetaData.Name = FName("SoundVolume");
	//	MetaData.DisplayText = NSLOCTEXT("MovieScene", "SoundVolume", "Sound Volume");
	//	Channels.Add(SoundVolume, MetaData, TMovieSceneExternalValue<float>());

	//	MetaData.Name = FName("PitchBend");
	//	MetaData.DisplayText = NSLOCTEXT("MovieScene", "PitchBend", "Pitch Bend");
	//	Channels.Add(PitchBend, MetaData, TMovieSceneExternalValue<float>());

	//	//MetaData.Name = FName("MidiNotes");
	//	//MetaData.DisplayText = NSLOCTEXT("MovieScene", "MidiNotes", "Midi Notes");
	//	//Channels.Add(MidiNotes, MetaData, TMovieSceneExternalValue<FLinkedMidiEvents>());

	//	ChannelProxy = MakeShared<FMovieSceneChannelProxy>(MoveTemp(Channels));
	//	return EMovieSceneChannelProxyType::Dynamic;
	//}

	FText GetSectionTitle() const;


	
protected:

	friend class UBkMovieSceneMidiTrack;

	UPROPERTY()
	TObjectPtr<UMovieSceneTrack> ParentTrack;


#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif


public:




private:

	double GetPlayerTimeAsSeconds(const UE::MovieScene::FEvaluationHookParams& Params) const;


	double UpdateSectionTime(const UE::MovieScene::FEvaluationHookParams& Params) const;


	double GetSectionTimeAhead(const UE::MovieScene::FEvaluationHookParams& Params) const;

	double GetPlayerTimeAhead(const UE::MovieScene::FEvaluationHookParams& Params) const;


	double GetDeltaTimeAsSeconds(const UE::MovieScene::FEvaluationHookParams& Params) const;

};
