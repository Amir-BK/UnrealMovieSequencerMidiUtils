#include "BkMovieSceneMidiTrack.h"

#include "MovieScene.h"


#include "BkMovieSceneMidiTrackSection.h"


UBkMovieSceneMidiTrackSection* UBkMovieSceneMidiTrack::AddNewMidiTrackOnRow(FSequencerMidiNotesTrack& InMidiDataPtr, FFrameNumber Time, int32 RowIndex, UMidiFile* InMidiFile)
{

	//check(DAWData);

	//MidiData = InMidiDataPtr;
	FSequencerMidiNotesTrack& NewSectionMidiData = MidiTracks[RowIndex];
	MidiFile = InMidiFile;

	FFrameRate FrameRate = GetTypedOuter<UMovieScene>()->GetTickResolution();
	FFrameTime DurationToUse = 1.f * FrameRate; // if all else fails, use 1 second duration



	//const float Duration = DAWData->SequenceDuration * .001f;
	const float Duration = MidiFile->GetSongMaps()->GetSongLengthMs() * .001f;

	DurationToUse = Duration * FrameRate;



	//add the section
	UBkMovieSceneMidiTrackSection* NewEvaluationSection = Cast<UBkMovieSceneMidiTrackSection>(CreateNewSection());
	NewEvaluationSection->MidiData = NewSectionMidiData;
	NewEvaluationSection->Midi = MidiFile;
	NewEvaluationSection->InitialPlacementOnRow(MidiSections, Time, DurationToUse.FrameNumber.Value, RowIndex);
	//NewEvaluationSection->DAWSequencerData = DAWData;
	NewEvaluationSection->TrackIndexInParentSession = RowIndex;

	MidiSections.Add(NewEvaluationSection);

	return NewEvaluationSection;
};

void UBkMovieSceneMidiTrack::ParseRawMidiEventsIntoNotesAndTracks(UMidiFile* InMidiFile)
{
	TArray<TTuple<int32, int32>> DiscoveredVoices; // for our purposes we want to clean the midi representation and place every unique combo of track/channel into a single track, which can be considered a voice

	int VoiceIndex = 0;
	//we discover the unique voices in the midi file as we're traversing it to tuild the note representation

	TMap<int32, FMidiEvent> UnlinkedNoteOns; // we store the note ons that have not been linked to a note off yet

	for (const auto& Track : InMidiFile->GetTracks())
	{
		const int InternalTrackIndex = VoiceIndex++;
		for (const auto& MidiEvent : Track.GetEvents())
		{
			switch (MidiEvent.GetMsg().Type)
			{
			case FMidiMsg::EType::Std:

				const int32 NoteNumber = MidiEvent.GetMsg().GetStdData1();

				if (MidiEvent.GetMsg().IsNoteOn())
				{
					//unlinkedNotes.Add(MidiEvent.GetMsg().GetStdData1(), MidiEvent);
					UnlinkedNoteOns.Add(NoteNumber, MidiEvent);
				}
				else 
				{
					if (UnlinkedNoteOns.Contains(NoteNumber))
					{
						
						//we check that they're the same channel
						if (UnlinkedNoteOns[NoteNumber].GetMsg().GetStdChannel() == MidiEvent.GetMsg().GetStdChannel())
						{
							//if we're here we have a valid note on and note off pair
							// remove the note on from the unlinked notes
							FMidiEvent NoteOn = UnlinkedNoteOns[NoteNumber];
							UnlinkedNoteOns.Remove(NoteNumber);

							//create a new note
							const float StartTick = NoteOn.GetTick();
							const float EndTick = MidiEvent.GetTick();
							
							const FSequencerMidiNote NewNote = { NoteNumber, StartTick, EndTick };

							//so we have an event, but we need to find the voice it belongs to
							const int32 VoiceHash = DiscoveredVoices.AddUnique(TTuple<int32, int32>(InternalTrackIndex, MidiEvent.GetMsg().GetStdChannel()));

							if(MidiTracks.Contains(VoiceHash))
							{
								MidiTracks[VoiceHash].Notes.Add(NewNote);
							}
							else
							{
								FSequencerMidiNotesTrack NewTrack;
								FString TrackName = *InMidiFile->GetTrack(InternalTrackIndex)->GetName();
								NewTrack.TrackName = FName(FString::Printf(TEXT("%d : %s"), VoiceHash, *TrackName)); //TODO
								NewTrack.TrackColor = FLinearColor::MakeRandomSeededColor(VoiceHash);
								NewTrack.Notes.Add(NewNote);
								NewTrack.TrackIndexInMidiFile = InternalTrackIndex;
								NewTrack.ChannelIndexInMidiFile = MidiEvent.GetMsg().GetStdChannel();
								MidiTracks.Add(VoiceHash, NewTrack);
							}

						}

					}
				}



			}
	
		}
	}



}

void UBkMovieSceneMidiTrack::CreateSectionsFromMidiTracks()
{
	for (const auto& Track : MidiTracks)
	{
		//print the name or something
		UE_LOG(LogTemp, Warning, TEXT("Track Name: %s"), *Track.Value.TrackName.ToString());
	}
}

UMovieSceneSection* UBkMovieSceneMidiTrack::CreateNewSection()
{
	return NewObject<UBkMovieSceneMidiTrackSection>(this, NAME_None, RF_Transactional);
}

void UBkMovieSceneMidiTrack::AddSection(UMovieSceneSection& Section)
{
	MidiSections.Add(Cast<UBkMovieSceneMidiTrackSection>(&Section));
}

bool UBkMovieSceneMidiTrack::SupportsType(TSubclassOf<UMovieSceneSection> SectionClass) const
{
	return SectionClass == UBkMovieSceneMidiTrackSection::StaticClass();
}

bool UBkMovieSceneMidiTrack::HasSection(const UMovieSceneSection& Section) const
{
	return MidiSections.Contains(&Section);
}

bool UBkMovieSceneMidiTrack::IsEmpty() const
{
	return MidiSections.Num() == 0;
}


void UBkMovieSceneMidiTrack::RemoveSection(UMovieSceneSection& Section)
{
	MidiSections.Remove(&Section);

}

void UBkMovieSceneMidiTrack::RemoveSectionAt(int32 SectionIndex)
{
	MidiSections.RemoveAt(SectionIndex);
	
}

const TArray<UMovieSceneSection*>& UBkMovieSceneMidiTrack::GetAllSections() const
{
	return MidiSections;
}

bool UBkMovieSceneMidiTrack::SupportsMultipleRows() const
{
	return true;
}




