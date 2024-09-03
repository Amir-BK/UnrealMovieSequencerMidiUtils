#include "BkMovieSceneMidiTrack.h"

#include "MovieScene.h"


#include "BkMovieSceneMidiTrackSection.h"


UBkMovieSceneMidiTrackSection* UBkMovieSceneMidiTrack::AddNewMidiTrackOnRow(FFrameNumber Time, int32 RowIndex, UMidiFile* InMidiFile)
{

	check(InMidiFile);

	MidiFile = InMidiFile;

	FFrameRate FrameRate = GetTypedOuter<UMovieScene>()->GetTickResolution();
	FFrameTime DurationToUse = 1.f * FrameRate; // if all else fails, use 1 second duration

	const float Duration = InMidiFile->GetSongMaps()->GetSongLengthMs() * .001f;

	DurationToUse = Duration * FrameRate;

	//add the section
	UBkMovieSceneMidiTrackSection* NewEvaluationSection = Cast<UBkMovieSceneMidiTrackSection>(CreateNewSection());
	NewEvaluationSection->ParentTrack = this;

	NewEvaluationSection->ParseRawMidiEventsIntoNotesAndChannels(InMidiFile);	
	
	NewEvaluationSection->InitialPlacementOnRow(MidiSections, Time, DurationToUse.FrameNumber.Value, RowIndex);


	MidiSections.Add(NewEvaluationSection);

	return NewEvaluationSection;
};



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
//
//#if WITH_EDITOR
//inline EMovieSceneSectionMovedResult UBkMovieSceneMidiTrack::OnSectionMoved(UMovieSceneSection& Section, const FMovieSceneSectionMovedParams& Params)
//{
//
//
//	auto MovedSectionNewOffset = Section.GetInclusiveStartFrame();
//	auto MovedSectionNewEnd = Section.GetExclusiveEndFrame();
//
//	FString MoveType;
//
//	switch (Params.MoveType)
//	{
//	case EPropertyChangeType::ArrayAdd:
//		MoveType = "ArrayAdd";
//		break;
//	case EPropertyChangeType::ArrayRemove:
//		MoveType = "ArrayRemove";
//		break;
//	case EPropertyChangeType::ArrayMove:
//		MoveType = "ArrayMove";
//		break;
//	case EPropertyChangeType::Duplicate:
//		MoveType = "Duplicate";
//		break;
//	case EPropertyChangeType::ValueSet:
//		MoveType = "ValueSet";
//		break;
//	case EPropertyChangeType::Interactive:
//		MoveType = "Interactive";
//		break;
//	case EPropertyChangeType::Redirected:
//		MoveType = "Redirected";
//		break;
//	case EPropertyChangeType::ToggleEditable:
//		MoveType = "ToggleEditable";
//		break;
//	default:
//			MoveType = "Unknown";
//			break;
//
//	}
//
//	UE_LOG(LogTemp, Log, TEXT("MovedSectionNewOffset: %d, MoveType: %s"), MovedSectionNewOffset.Value, *MoveType);
//
//	for (auto& MidiSection : MidiSections)
//	{
//		if (MidiSection != &Section)
//		{
//			MidiSection->SetStartFrame(MovedSectionNewOffset);
//			MidiSection->SetEndFrame(MovedSectionNewEnd);
//		}
//
//	}
//
//	return EMovieSceneSectionMovedResult::None;
//}
//#endif




