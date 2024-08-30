#include "BkMovieSceneMidiTrackSection.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "LevelSequencePlayer.h"
#include "MovieScene.h"
//#include "MidiBroadcasters/MidiBroadcasterPlayHead.h"
//#include "Tracks/MidiAssetSceneTrack.h"


void UBkMovieSceneMidiTrackSection::MarkBars()
{
	UMovieScene* MovieScene = GetTypedOuter<UMovieScene>();

	FFrameRate FrameRate = MovieScene->GetTickResolution();

	//FFrameTime SectionStartTime = GetInclusiveStartFrame();
	//FFrameTime SectionEndTime = GetExclusiveEndFrame();

	const float SectionStartTimeSeconds = FrameRate.AsSeconds(GetInclusiveStartFrame());

	const auto& BarMap = Midi->GetSongMaps()->GetBarMap();
	const auto& SongsMap = Midi->GetSongMaps();
	const float FirstTickOfFirstBar = BarMap.MusicTimestampBarToTick(0);
	const float LastTickOfLastBar = Midi->GetLastEventTick();

	MovieScene->DeleteMarkedFrames();

	float BarTick = 0;
	int i = 0;
	while (BarTick <= LastTickOfLastBar)
	{
		const auto& BarTime = SongsMap->TickToMs(BarTick) * .001f + SectionStartTimeSeconds;
		FFrameTime BarFrameTime = FFrameTime(FrameRate.AsFrameTime(BarTime));


		auto MarkedFrameTest = FMovieSceneMarkedFrame(FFrameNumber(BarFrameTime.FrameNumber));
		MarkedFrameTest.Label = FString::Printf(TEXT("Bar %d"), ++i);
		MarkedFrameTest.Color = FLinearColor::Green;
		MovieScene->AddMarkedFrame(MarkedFrameTest);

		BarTick += SongsMap->SubdivisionToMidiTicks(EMidiClockSubdivisionQuantization::Bar, BarTick);
	}

}

void UBkMovieSceneMidiTrackSection::MarkSubdivisionsInRange()
{
	//UMovieScene* MovieScene = GetTypedOuter<UMovieScene>();

	//FFrameRate FrameRate = MovieScene->GetTickResolution();

	////FFrameTime SectionStartTime = GetInclusiveStartFrame();
	////FFrameTime SectionEndTime = GetExclusiveEndFrame();

	//const auto& BarMap = DAWSequencerData->HarmonixMidiFile->GetSongMaps()->GetBarMap();
	//const float FirstTickOfFirstBar = BarMap.MusicTimestampBarToTick(0);
	//const float LastTickOfLastBar = DAWSequencerData->HarmonixMidiFile->GetLastEventTick();


	//auto SelectionRange = MovieScene->GetSelectionRange();

	//FFrameTime SectionStartTime = SelectionRange.GetLowerBoundValue();
	//FFrameTime SectionEndTime = SelectionRange.GetUpperBoundValue();
	//

	//MovieScene->DeleteMarkedFrames();

	//auto RangeStartSeconds = FrameRate.AsSeconds(SectionStartTime);
	//auto RangeEndSeconds = FrameRate.AsSeconds(SectionEndTime);

	//UE_LOG(LogTemp, Warning, TEXT("RangeStartSeconds %f, RangeEndSeconds %f"), RangeStartSeconds, RangeEndSeconds);

	//float FirstTickInSelectionRange = DAWSequencerData->HarmonixMidiFile->GetSongMaps()->MsToTick(RangeStartSeconds * 1000);
	//float LastTickInSelectionRange = DAWSequencerData->HarmonixMidiFile->GetSongMaps()->MsToTick(RangeEndSeconds * 1000);

	//UE_LOG(LogTemp, Warning, TEXT("FirstTickInSelectionRange %f, LastTickInSelectionRange %f"), FirstTickInSelectionRange, LastTickInSelectionRange);

	//const auto& SongsMap = DAWSequencerData->HarmonixMidiFile->GetSongMaps();

	//float FirstSubdivisionInSelectionRange = SongsMap->QuantizeTickToNearestSubdivision(FMath::FloorToInt32(FirstTickInSelectionRange), EMidiFileQuantizeDirection::Up, MusicSubdivision);
	//float LastSubdivisionInSelectionRange = SongsMap->QuantizeTickToNearestSubdivision(FMath::FloorToInt32(LastTickInSelectionRange), EMidiFileQuantizeDirection::Down, MusicSubdivision);
	//	//DAWSequencerData->HarmonixMidiFile->GetSongMaps()->SubdivisionToMidiTicks(MusicSubdivision, FMath::FloorToInt32(LastTickInSelectionRange));

	//UE_LOG(LogTemp, Warning, TEXT("FirstSubdivisionInSelectionRange %f, LastSubdivisionInSelectionRange %f"), FirstSubdivisionInSelectionRange, LastSubdivisionInSelectionRange);

	//while (FirstSubdivisionInSelectionRange <= LastSubdivisionInSelectionRange)
	//{
	//
	//	const auto& SubdivisionTime = DAWSequencerData->HarmonixMidiFile->GetSongMaps()->TickToMs(FirstSubdivisionInSelectionRange);
	//	FFrameTime SubdivisionFrameTime = FFrameTime(FrameRate.AsFrameTime(SubdivisionTime * .001f));

	//	auto MarkedFrameTest = FMovieSceneMarkedFrame(FFrameNumber(SubdivisionFrameTime.FrameNumber));
	//	MarkedFrameTest.Label = FString::Printf(TEXT("Subdivision %d"), FirstSubdivisionInSelectionRange);
	//	MarkedFrameTest.Color = FLinearColor::Gray;
	//	MovieScene->AddMarkedFrame(MarkedFrameTest);

	//	FirstSubdivisionInSelectionRange += DAWSequencerData->HarmonixMidiFile->GetSongMaps()->SubdivisionToMidiTicks(MusicSubdivision, FirstSubdivisionInSelectionRange);
	//
	//}

	////float FirstTickOfSubdivision = DAWSequencerData->HarmonixMidiFile->GetSongMaps()->Ca

	//float BarTick = DAWSequencerData->HarmonixMidiFile->GetSongMaps()->MsToTick(SectionStartTime.AsDecimal());
	//float BarEndTick = DAWSequencerData->HarmonixMidiFile->GetSongMaps()->MsToTick(SectionEndTime.AsDecimal());

	////float BarTick = DAWSequencerData->HarmonixMidiFile->GetSongMaps()->SubdivisionToMidiTicks(MusicSubdivision, SectionStartTime.AsDecimal());;
	////float BarEndTick = DAWSequencerData->HarmonixMidiFile->GetSongMaps()->SubdivisionToMidiTicks(MusicSubdivision, SectionEndTime.AsDecimal());;

	//while (BarTick <= BarEndTick)
	//{
	//	const auto& BarTime = DAWSequencerData->HarmonixMidiFile->GetSongMaps()->TickToMs(BarTick);
	//	FFrameTime BarFrameTime = FFrameTime(FrameRate.AsFrameTime(BarTime * .001f));


	//	auto MarkedFrameTest = FMovieSceneMarkedFrame(FFrameNumber(BarFrameTime.FrameNumber));
	//	//MarkedFrameTest.Label = FString::Printf(TEXT("Bar %d"), ++i);
	//	MarkedFrameTest.Color = FLinearColor::Gray;
	//	MovieScene->AddMarkedFrame(MarkedFrameTest);

	//	BarTick += DAWSequencerData->HarmonixMidiFile->GetSongMaps()->SubdivisionToMidiTicks(MusicSubdivision, BarTick);
	//}
}

void UBkMovieSceneMidiTrackSection::MarkNotesInRange()
{
	/*UMovieScene* MovieScene = GetTypedOuter<UMovieScene>();

	FFrameRate FrameRate = MovieScene->GetTickResolution();

	auto SelectionRange = MovieScene->GetSelectionRange();

	FFrameTime SectionStartTime = SelectionRange.GetLowerBoundValue();
	FFrameTime SectionEndTime = SelectionRange.GetUpperBoundValue();

	const auto& BarMap = DAWSequencerData->HarmonixMidiFile->GetSongMaps()->GetBarMap();

	MovieScene->DeleteMarkedFrames();

	auto LinkedNotesTracks = DAWSequencerData->LinkedNoteDataMap;
	const auto& Track = LinkedNotesTracks[TrackIndexInParentSession];
	auto TrackColor = DAWSequencerData->GetTracksDisplayOptions(TrackIndexInParentSession).trackColor;

	for (const auto& Note : Track.LinkedNotes)
	{
		const auto& NoteTime = DAWSequencerData->HarmonixMidiFile->GetSongMaps()->TickToMs(Note.StartTick);

		FFrameTime NoteFrameTime = FFrameTime(FrameRate.AsFrameTime(NoteTime * .001f));

		if (NoteFrameTime >= SectionStartTime && NoteFrameTime <= SectionEndTime)
		{
			auto MarkedFrameTest = FMovieSceneMarkedFrame(FFrameNumber(NoteFrameTime.FrameNumber));
			MarkedFrameTest.Comment = FString::Printf(TEXT("Note %d"), Note.pitch);
			MarkedFrameTest.Color = TrackColor;
			MovieScene->AddMarkedFrame(MarkedFrameTest);
		}
	}*/
}

UBkMovieSceneMidiTrackSection::UBkMovieSceneMidiTrackSection(const FObjectInitializer& ObjInit) : Super(ObjInit)
{
}


#if WITH_EDITOR
FText UBkMovieSceneMidiTrackSection::GetSectionTitle() const
{
	if (!IsValid(ParentTrack))
	{
		return FText::FromString("No Parent Track");
	}
	
	return ParentTrack->GetTrackRowDisplayName(TrackIndexInParentSession);
}
#endif




