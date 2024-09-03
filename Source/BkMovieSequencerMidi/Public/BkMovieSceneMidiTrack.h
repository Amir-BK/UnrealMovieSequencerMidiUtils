#pragma once

#include "CoreMinimal.h"

#include "MovieSceneNameableTrack.h"
#include "HarmonixMidi/MidiFile.h"
#include "HarmonixMetasound/DataTypes/MidiEventInfo.h"
#include "BkMovieSequencerMidi.h"
#include "BkMovieSceneMidiTrackSection.h"
#include "BkMovieSceneMidiTrack.generated.h"

//DECLARE_DYNAMIC_DELEGATE_OneParam(FOnMidiNote, FMidiEventInfo, Note);

//represents an entire midi file where each of the internal tracks are represented as sections
UCLASS()
class BKMOVIESEQUENCERMIDI_API UBkMovieSceneMidiTrack : public UMovieSceneNameableTrack
{
	GENERATED_BODY()


public:

	UFUNCTION(BlueprintCallable, Category = "Midi", meta = (AutoCreateRefTerm = "InDelegate", Keywords = "Event, Quantization, DAW"))
	void SubscribeToMidiNoteEvents(FOnMidiNote InDelegate) {};

	virtual UBkMovieSceneMidiTrackSection* AddNewMidiTrackOnRow(FFrameNumber Time, int32 RowIndex, UMidiFile* InMidiFile);

	//begin UMovieSceneTrack interface

	virtual UMovieSceneSection* CreateNewSection() override;
	virtual void AddSection(UMovieSceneSection& Section) override;
	virtual bool SupportsType(TSubclassOf<UMovieSceneSection> SectionClass) const override;
	virtual bool HasSection(const UMovieSceneSection& Section) const override;
	virtual void RemoveSection(UMovieSceneSection& Section) override;
	virtual void RemoveSectionAt(int32 SectionIndex) override;
	virtual bool IsEmpty() const override;
	virtual const TArray<UMovieSceneSection*>& GetAllSections() const override;
	virtual bool SupportsMultipleRows() const override;
/*
#if WITH_EDITOR

	virtual EMovieSceneSectionMovedResult OnSectionMoved(UMovieSceneSection& Section, const FMovieSceneSectionMovedParams& Params) override;

#endif

*/

	//END UMovieSceneTrack Interface

	UPROPERTY()
	TMap<int32, FSequencerMidiNotesTrack> MidiTracks;


	private:
	
		/** List of all root audio sections */
	UPROPERTY()
	TArray<TObjectPtr<UMovieSceneSection>> MidiSections;

	UPROPERTY()
	TObjectPtr<UMidiFile> MidiFile;


	


};
