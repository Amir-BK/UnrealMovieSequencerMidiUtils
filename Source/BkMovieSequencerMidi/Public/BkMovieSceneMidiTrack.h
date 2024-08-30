#pragma once

#include "CoreMinimal.h"

#include "MovieSceneNameableTrack.h"
#include "HarmonixMidi/MidiFile.h"
#include "BkMovieSequencerMidi.h"
#include "BkMovieSceneMidiTrackSection.h"
#include "BkMovieSceneMidiTrack.generated.h"



//represents an entire midi file where each of the internal tracks are represented as sections
UCLASS()
class BKMOVIESEQUENCERMIDI_API UBkMovieSceneMidiTrack : public UMovieSceneNameableTrack
{
	GENERATED_BODY()


public:

	virtual UBkMovieSceneMidiTrackSection* AddNewMidiTrackOnRow(FSequencerMidiNotesTrack& MidiDataPtr, FFrameNumber Time, int32 RowIndex, UMidiFile* InMidiFile);

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

#if WITH_EDITOR
	/**
	 * we override this function to update the start frame of all the midi sections when the track is moved
	 */
	virtual EMovieSceneSectionMovedResult OnSectionMoved(UMovieSceneSection& Section, const FMovieSceneSectionMovedParams& Params) override;

#endif

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
