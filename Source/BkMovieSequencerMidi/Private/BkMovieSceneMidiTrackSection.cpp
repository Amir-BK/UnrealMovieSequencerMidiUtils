#include "BkMovieSceneMidiTrackSection.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "LevelSequencePlayer.h"
#include "MovieScene.h"

#if WITH_EDITOR

void UBkMovieSceneMidiTrackSection::MarkBars()
{
	UMovieScene* MovieScene = GetTypedOuter<UMovieScene>();

	FFrameRate FrameRate = MovieScene->GetTickResolution();

	
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
		MarkedFrames.Add(MovieScene->AddMarkedFrame(MarkedFrameTest));

		BarTick += SongsMap->SubdivisionToMidiTicks(EMidiClockSubdivisionQuantization::Bar, BarTick);
	}

}

void UBkMovieSceneMidiTrackSection::MarkSubdivisionsInRange()
{
	
	UMovieScene* MovieScene = GetTypedOuter<UMovieScene>();

	FFrameRate FrameRate = MovieScene->GetTickResolution();


	const float SectionStartTimeSeconds = FrameRate.AsSeconds(GetInclusiveStartFrame());

	const auto& BarMap = Midi->GetSongMaps()->GetBarMap();
	const auto& SongsMap = Midi->GetSongMaps();
	const float FirstTickOfFirstBar = BarMap.MusicTimestampBarToTick(0);
	const float LastTickOfLastBar = Midi->GetLastEventTick();
	
	
	auto SelectionRange = MovieScene->GetSelectionRange();

	FFrameTime SectionStartTime = SelectionRange.GetLowerBoundValue();
	FFrameTime SectionEndTime = SelectionRange.GetUpperBoundValue();

	MovieScene->DeleteMarkedFrames();

	auto RangeStartSeconds = FrameRate.AsSeconds(SectionStartTime);
	auto RangeEndSeconds = FrameRate.AsSeconds(SectionEndTime);


	float FirstTickInSelectionRange = SongsMap->MsToTick(RangeStartSeconds * 1000);
	float LastTickInSelectionRange = SongsMap->MsToTick(RangeEndSeconds * 1000);

	float FirstSubdivisionInSelectionRange = SongsMap->QuantizeTickToNearestSubdivision(FMath::FloorToInt32(FirstTickInSelectionRange), EMidiFileQuantizeDirection::Up, MusicSubdivision);
	float LastSubdivisionInSelectionRange = SongsMap->QuantizeTickToNearestSubdivision(FMath::FloorToInt32(LastTickInSelectionRange), EMidiFileQuantizeDirection::Down, MusicSubdivision);

	while (FirstSubdivisionInSelectionRange <= LastSubdivisionInSelectionRange)
	{
	
		const auto& SubdivisionTime = SongsMap->TickToMs(FirstSubdivisionInSelectionRange);
		FFrameTime SubdivisionFrameTime = FFrameTime(FrameRate.AsFrameTime(SubdivisionTime * .001f));

		auto MarkedFrameTest = FMovieSceneMarkedFrame(FFrameNumber(SubdivisionFrameTime.FrameNumber));
		MarkedFrameTest.Label = FString::Printf(TEXT("Subdivision %d"), FirstSubdivisionInSelectionRange);
		MarkedFrameTest.Color = FLinearColor::Gray;
		MovieScene->AddMarkedFrame(MarkedFrameTest);

		FirstSubdivisionInSelectionRange += SongsMap->SubdivisionToMidiTicks(MusicSubdivision, FirstSubdivisionInSelectionRange);
	
	}

}

void UBkMovieSceneMidiTrackSection::MarkNotesInRange()
{
	UMovieScene* MovieScene = GetTypedOuter<UMovieScene>();

	FFrameRate FrameRate = MovieScene->GetTickResolution();

	auto SelectionRange = MovieScene->GetSelectionRange();

	FFrameTime SectionStartTime = SelectionRange.GetLowerBoundValue();
	FFrameTime SectionEndTime = SelectionRange.GetUpperBoundValue();

	const auto& BarMap = Midi->GetSongMaps()->GetBarMap();
	const auto& SongsMap = Midi->GetSongMaps();
	

	MovieScene->DeleteMarkedFrames();

	for (const auto& Note : MidiData.Notes)
	{
		const auto& NoteTime = SongsMap->TickToMs(Note.StartTick);
		FFrameTime NoteFrameTime = FFrameTime(FrameRate.AsFrameTime(NoteTime * .001f));

		if (NoteFrameTime >= SectionStartTime && NoteFrameTime <= SectionEndTime)
		{
			auto MarkedFrameTest = FMovieSceneMarkedFrame(FFrameNumber(NoteFrameTime.FrameNumber));
			MarkedFrameTest.Label = FString::Printf(TEXT("Note %d"), Note.NoteNumber);
			MarkedFrameTest.Color = NoteColor;
			MovieScene->AddMarkedFrame(MarkedFrameTest);
		}

	}

}

#endif // WITH_EDITOR

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




