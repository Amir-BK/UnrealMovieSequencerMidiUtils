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


		auto MarkedFrame = FMovieSceneMarkedFrame(FFrameNumber(BarFrameTime.FrameNumber));
		MarkedFrame.Label = FString::Printf(TEXT("Bar %d"), ++i);
		MarkedFrame.Color = FLinearColor::Green;
		MarkedFrames.Add(MovieScene->AddMarkedFrame(MarkedFrame));

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

	FFrameTime RangeToMarkStartFrame;
	FFrameTime RangeToMarkEndFrame;

	if (!bMarkOnlyInSelectionRange || SelectionRange.IsEmpty())
	{
		RangeToMarkStartFrame = SelectionRange.GetLowerBoundValue();
		RangeToMarkEndFrame = SelectionRange.GetUpperBoundValue();
	}
	else
	{
		RangeToMarkStartFrame = GetInclusiveStartFrame();
		RangeToMarkEndFrame = GetExclusiveEndFrame();

	}

	MovieScene->DeleteMarkedFrames();

	float FirstTickInSelectionRange = SongsMap->MsToTick(FrameRate.AsSeconds(RangeToMarkStartFrame) * 1000);
	float LastTickInSelectionRange = SongsMap->MsToTick(FrameRate.AsSeconds(RangeToMarkEndFrame) * 1000);

	float FirstSubdivisionInSelectionRange = SongsMap->QuantizeTickToNearestSubdivision(FMath::FloorToInt32(FirstTickInSelectionRange), EMidiFileQuantizeDirection::Up, MusicSubdivision);
	float LastSubdivisionInSelectionRange = SongsMap->QuantizeTickToNearestSubdivision(FMath::FloorToInt32(LastTickInSelectionRange), EMidiFileQuantizeDirection::Down, MusicSubdivision);

	while (FirstSubdivisionInSelectionRange <= LastSubdivisionInSelectionRange)
	{
	
		const auto& SubdivisionTime = SongsMap->TickToMs(FirstSubdivisionInSelectionRange) * .001f + SectionStartTimeSeconds;
		FFrameTime SubdivisionFrameTime = FFrameTime(FrameRate.AsFrameTime(SubdivisionTime));

		auto MarkedFrame = FMovieSceneMarkedFrame(FFrameNumber(SubdivisionFrameTime.FrameNumber));
		MarkedFrame.Label = FString::Printf(TEXT("Subdivision %d"), FirstSubdivisionInSelectionRange);
		MarkedFrame.Color = FLinearColor::Gray;
		MovieScene->AddMarkedFrame(MarkedFrame);

		FirstSubdivisionInSelectionRange += SongsMap->SubdivisionToMidiTicks(MusicSubdivision, FirstSubdivisionInSelectionRange);
	
	}

}

#endif // WITH_EDITOR

void UBkMovieSceneMidiTrackSection::RebuildNoteKeyFrames()
{
	if (CanModify())
	{
		Modify();
		//const FScopedTransaction Transaction(INVTEXT("Rebuild Note Key Frames"));

		const auto& SongsMap = Midi->GetSongMaps();
		FFrameRate FrameRate = GetTypedOuter<UMovieScene>()->GetTickResolution();

		for (int i = 0; i < MidiChannels.Num(); i++)
		{
			auto& Channel = MidiChannels[i];
			auto& NoteChannel = MidiNoteChannels[i];
			NoteChannel.Reset();

			TArray<int> NotePitchesChannels;
			TArray<FFrameNumber> NoteTimesChannels;

			for (const auto& Note : Channel.Notes)
			{
				const auto& NoteTime = SongsMap->TickToMs(Note.StartTick) * .001f;
				const auto NoteFrameTime = FFrameTime(NoteTime * FrameRate);

				//NoteChannel.AddKey(NoteFrameTime.FrameNumber, Note.NoteNumber);
				NoteTimesChannels.Add(NoteFrameTime.FrameNumber);
				NotePitchesChannels.Add(Note.NoteNumber);
			}

			NoteChannel.AddKeys(NoteTimesChannels, NotePitchesChannels);
		}
	}
}

void UBkMovieSceneMidiTrackSection::ParseRawMidiEventsIntoNotesAndChannels(UMidiFile* InMidiFile)
{
	TArray<TTuple<int32, int32>> DiscoveredVoices; // for our purposes we want to clean the midi representation and place every unique combo of track/channel into a single track, which can be considered a voice

	int VoiceIndex = 0;
	//we discover the unique voices in the midi file as we're traversing it to tuild the note representation

	TMap<int32, FMidiEvent> UnlinkedNoteOns; // we store the note ons that have not been linked to a note off yet
	Midi = InMidiFile;

	//we keep track of the highest and lowest note pitch to determine the range of the midi file
	MaxNotePitch = 0;
	MinNotePitch = 127;

	for (const auto& Track : InMidiFile->GetTracks())
	{
		const int InternalTrackIndex = VoiceIndex++;
		for (const auto& MidiEvent : Track.GetEvents())
		{
			switch (MidiEvent.GetMsg().Type)
			{
			case FMidiMsg::EType::Std:

				if (MidiEvent.GetMsg().IsNoteOn())
				{
				const int32 NoteNumber = MidiEvent.GetMsg().GetStdData1();
	
					UnlinkedNoteOns.Add(NoteNumber, MidiEvent);
				}
				else
				{
					const int32 NoteNumber = MidiEvent.GetMsg().GetStdData1();
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

							if (MidiChannels.IsValidIndex(VoiceHash))
							{
								MidiChannels[VoiceHash].Notes.Add(NewNote);
							}
							else
							{
								FSequencerMidiNotesTrack NewTrack;
								FString TrackName = *InMidiFile->GetTrack(InternalTrackIndex)->GetName();
								NewTrack.Name = FString::Printf(TEXT("%d : %s"), VoiceHash, *TrackName); //TODO
								NewTrack.TrackColor = FLinearColor::MakeRandomSeededColor(VoiceHash);
								NewTrack.Notes.Add(NewNote);
								NewTrack.TrackIndexInMidiFile = InternalTrackIndex;
								NewTrack.ChannelIndexInMidiFile = MidiEvent.GetMsg().GetStdChannel();
								//MidiTracks.Add(VoiceHash, NewTrack);
								MidiChannels.Add(NewTrack);
							}

							//we keep track of the highest and lowest note pitch
							if (NoteNumber > MaxNotePitch)
							{
								MaxNotePitch = NoteNumber;
							}
							if (NoteNumber < MinNotePitch)
							{
								MinNotePitch = NoteNumber;
							}
						}

					}

				}

				break;
			default:

				break;

			}
		}
	}

	//create channels

	for (int i = 0; i < MidiChannels.Num(); i++)
	{
		MidiNoteChannels.Add(FMovieSceneIntegerChannel());
	}
	
	RebuildNoteKeyFrames();
}



UBkMovieSceneMidiTrackSection::UBkMovieSceneMidiTrackSection(const FObjectInitializer& ObjInit) : Super(ObjInit)
{
}

EMovieSceneChannelProxyType UBkMovieSceneMidiTrackSection::CacheChannelProxy()
{
	FMovieSceneChannelProxyData Channels;

	for (int i = 0; i < MidiNoteChannels.Num(); i++)
	{
		FMovieSceneChannelMetaData MetaData;
		MetaData.bCanCollapseToTrack = false;
		MetaData.Name = *MidiChannels[i].Name;
		MetaData.DisplayText = FText::FromString(MidiChannels[i].Name);
		MetaData.Color = MidiChannels[i].TrackColor;
		
		Channels.Add(MidiNoteChannels[i], MetaData, TMovieSceneExternalValue<int>());
	}

	ChannelProxy = MakeShared<FMovieSceneChannelProxy>(MoveTemp(Channels));
	
	return EMovieSceneChannelProxyType::Dynamic;
}

#if WITH_EDITOR
FText UBkMovieSceneMidiTrackSection::GetSectionTitle() const
{

	return FText::FromName(Midi->GetFName());
}
void UBkMovieSceneMidiTrackSection::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property != nullptr)
	{
		const FName PropertyName = PropertyChangedEvent.Property->GetFName();
		if (PropertyName == GET_MEMBER_NAME_CHECKED(UBkMovieSceneMidiTrackSection, Midi))
		{
			ParseRawMidiEventsIntoNotesAndChannels(Midi);
		}
		//if colors or track names changed, cache proxy
		if (PropertyName == GET_MEMBER_NAME_CHECKED(FSequencerMidiNotesTrack, TrackColor) || PropertyName == GET_MEMBER_NAME_CHECKED(FSequencerMidiNotesTrack, Name))
		{
			CacheChannelProxy();
		}


	}


}
#endif




