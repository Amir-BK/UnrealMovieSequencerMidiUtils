#pragma once

#include "CoreMinimal.h"

#include "MovieSceneNameableTrack.h"
#include "HarmonixMidi/MidiFile.h"


//#include "M2SoundGraphData.h"
#include "BkMovieSceneMidiTrack.generated.h"

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
	FName TrackName = NAME_None;

	UPROPERTY(EditAnywhere, Category = "Midi")
	FLinearColor TrackColor = FLinearColor::White;

	UPROPERTY()
	int32 TrackIndexInMidiFile = INDEX_NONE;

	UPROPERTY()
	int32 ChannelIndexInMidiFile = INDEX_NONE;
};

//represents an entire midi file where each of the internal tracks are represented as sections
UCLASS()
class BKMOVIESEQUENCERMIDI_API UBkMovieSceneMidiTrack : public UMovieSceneNameableTrack
{
	GENERATED_BODY()

private:

public:

	//TObjectPtr<UDAWSequencerData> DAWSequencerData;



public:

	virtual UMovieSceneSection* AddNewDAWDataOnRow(UObject* DAWData, FFrameNumber Time, int32 RowIndex);

	virtual void ParseRawMidiEventsIntoNotesAndTracks(UMidiFile* InMidiFile);
	virtual void CreateSectionsFromMidiTracks();

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

	//END UMovieSceneTrack Interface



	private:
		private:

			/** List of all root audio sections */
			UPROPERTY()
			TArray<TObjectPtr<UMovieSceneSection>> DAWSections;

			UPROPERTY()
			TObjectPtr<UMidiFile> MidiFile;

			UPROPERTY()
			TMap<int32, FSequencerMidiNotesTrack> MidiTracks;

};
