#include "BkMovieSceneMidiTrackSection.h"
#include "Channels/MovieSceneChannelProxy.h"
#include “Misc/EngineVersionComparison.h”
#include "LevelSequencePlayer.h"
#include "MovieScene.h"

namespace BkMovieSceneMidiTrackSectionUtils
{
	FFrameNumber GetStartOffsetAtTrimTime(FQualifiedFrameTime TrimTime, FFrameNumber StartOffset, FFrameNumber StartFrame)
	{
		return StartOffset + TrimTime.Time.FrameNumber - StartFrame;
	}
}

#if WITH_EDITOR

void UBkMovieSceneMidiTrackSection::MarkBars()
{
	UMovieScene* MovieScene = GetTypedOuter<UMovieScene>();

	FFrameRate FrameRate = MovieScene->GetTickResolution();

	const float SectionStartTimeSeconds = FrameRate.AsSeconds(GetInclusiveStartFrame());
	const auto& SectionStartOffsetSeconds = FrameRate.AsSeconds(GetStartOffset());

	const auto& BarMap = Midi->GetSongMaps()->GetBarMap();
	const auto& SongsMap = Midi->GetSongMaps();
	const float FirstTickOfFirstBar = BarMap.MusicTimestampBarToTick(0);
	const float LastTickOfLastBar = Midi->GetLastEventTick();

	MovieScene->DeleteMarkedFrames();

	float BarTick = 0;
	int i = 0;
	while (BarTick <= LastTickOfLastBar)
	{
		const auto& BarTime = SongsMap->TickToMs(BarTick) * .001f - SectionStartOffsetSeconds;

		if (BarTime >= 0)
		{
			FFrameTime BarFrameTime = FFrameTime(FrameRate.AsFrameTime(BarTime + SectionStartTimeSeconds));
			auto MarkedFrame = FMovieSceneMarkedFrame(FFrameNumber(BarFrameTime.FrameNumber));
			MarkedFrame.Label = FString::Printf(TEXT("Bar %d"), ++i);
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION == 4
			MarkedFrame.Color = FLinearColor::Green;
#else
			MarkedFrame.CustomColor = FLinearColor::Green;
#endif
			
			MarkedFrames.Add(MovieScene->AddMarkedFrame(MarkedFrame));
		}
		else {
			i++;
		}

		BarTick += SongsMap->SubdivisionToMidiTicks(EMidiClockSubdivisionQuantization::Bar, BarTick);
	}
}

void UBkMovieSceneMidiTrackSection::MarkSubdivisions()
{
	UMovieScene* MovieScene = GetTypedOuter<UMovieScene>();

	FFrameRate FrameRate = MovieScene->GetTickResolution();

	const float SectionStartTimeSeconds = FrameRate.AsSeconds(GetInclusiveStartFrame());
	const auto& SectionStartOffsetSeconds = FrameRate.AsSeconds(GetStartOffset());

	const auto& BarMap = Midi->GetSongMaps()->GetBarMap();
	const auto& SongsMap = Midi->GetSongMaps();
	const float FirstTickOfFirstBar = BarMap.MusicTimestampBarToTick(0);
	const float LastTickOfLastBar = Midi->GetLastEventTick();

	MovieScene->DeleteMarkedFrames();

	float SubdivTick = 0;

	while (SubdivTick <= LastTickOfLastBar)
	{
		const auto& SubdivTime = SongsMap->TickToMs(SubdivTick) * .001f  - SectionStartOffsetSeconds;

		if (SubdivTime >= 0)
		{
			FFrameTime BarFrameTime = FFrameTime(FrameRate.AsFrameTime(SubdivTime + SectionStartTimeSeconds));
			auto MarkedFrame = FMovieSceneMarkedFrame(FFrameNumber(BarFrameTime.FrameNumber));
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION == 4
			MarkedFrame.Color = FLinearColor::Gray;
#else
			MarkedFrame.CustomColor = FLinearColor::Gray;
#endif

			MarkedFrames.Add(MovieScene->AddMarkedFrame(MarkedFrame));
		}


		SubdivTick += SongsMap->SubdivisionToMidiTicks(MusicSubdivision, SubdivTick);
	}
}

#endif // WITH_EDITOR

void UBkMovieSceneMidiTrackSection::RebuildNoteKeyFrames()
{
#if WITH_EDITOR
	if (CanModify())
	{
		Modify();
#endif

		const auto& SongsMap = Midi->GetSongMaps();
		FFrameRate FrameRate = GetTypedOuter<UMovieScene>()->GetTickResolution();
		const float SectionStartTime = FrameRate.AsSeconds(GetInclusiveStartFrame());
		const auto& SectionStartOffsetSeconds = FrameRate.AsSeconds(GetStartOffset());

		for (int i = 0; i < MidiChannels.Num(); i++)
		{
			auto& Channel = MidiChannels[i];
			auto& NoteChannel = MidiNoteChannels[i];
			NoteChannel.Reset();

			TArray<int> NotePitchesChannels;
			TArray<FFrameNumber> NoteTimesChannels;

			for (const auto& Note : Channel.Notes)
			{
				
				const float Tick = bMarkNoteOffs ? Note.EndTick : Note.StartTick;
				const auto& NoteTime = SongsMap->TickToMs(Tick) * .001f + SectionStartTime - SectionStartOffsetSeconds;
				const auto NoteFrameTime = FFrameTime(NoteTime * FrameRate);

				//NoteChannel.AddKey(NoteFrameTime.FrameNumber, Note.NoteNumber);
				NoteTimesChannels.Add(NoteFrameTime.FrameNumber);
				NotePitchesChannels.Add(Note.NoteNumber);
			}

			NoteChannel.AddKeys(NoteTimesChannels, NotePitchesChannels);
		}
#if WITH_EDITOR
	}
#endif
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

void UBkMovieSceneMidiTrackSection::TrimSection(FQualifiedFrameTime TrimTime, bool bTrimLeft, bool bDeleteKeys)
{
	SetFlags(RF_Transactional);

	if (TryModify())
	{
		if (bTrimLeft)
		{
			using namespace BkMovieSceneMidiTrackSectionUtils;
			StartFrameOffset = HasStartFrame() ? GetStartOffsetAtTrimTime(TrimTime, StartFrameOffset, GetInclusiveStartFrame()) : 0;
		}

		Super::TrimSection(TrimTime, bTrimLeft, bDeleteKeys);
	}
}

UMovieSceneSection* UBkMovieSceneMidiTrackSection::SplitSection(FQualifiedFrameTime SplitTime, bool bDeleteKeys)
{
	const FFrameNumber InitialStartFrameOffset = StartFrameOffset;

	using namespace BkMovieSceneMidiTrackSectionUtils;

	const FFrameNumber NewOffset = HasStartFrame() ? GetStartOffsetAtTrimTime(SplitTime, StartFrameOffset, GetInclusiveStartFrame()) : 0;

	UMovieSceneSection* NewSection = Super::SplitSection(SplitTime, bDeleteKeys);
	if (NewSection != nullptr)
	{
		auto* NewAudioSection = Cast<UBkMovieSceneMidiTrackSection>(NewSection);
		NewAudioSection->StartFrameOffset = NewOffset;
	}

	//Restore the initial start frame offset
	StartFrameOffset = InitialStartFrameOffset;

	return NewSection;
}

TOptional<FFrameTime> UBkMovieSceneMidiTrackSection::GetOffsetTime() const
{
	return TOptional<FFrameTime>(StartFrameOffset);
}

void UBkMovieSceneMidiTrackSection::MigrateFrameTimes(FFrameRate SourceRate, FFrameRate DestinationRate)
{
	if (StartFrameOffset.Value > 0)
	{
		FFrameNumber NewStartFrameOffset = ConvertFrameTime(FFrameTime(StartFrameOffset), SourceRate, DestinationRate).FloorToFrame();
		StartFrameOffset = NewStartFrameOffset;
	}
}

EMovieSceneChannelProxyType UBkMovieSceneMidiTrackSection::CacheChannelProxy()
{
	FMovieSceneChannelProxyData Channels;
#if WITH_EDITOR

	for (int i = 0; i < MidiNoteChannels.Num(); i++)
	{
		FMovieSceneChannelMetaData MetaData;
		MetaData.bCanCollapseToTrack = false;
		MetaData.Name = *MidiChannels[i].Name;
		MetaData.DisplayText = FText::FromString(MidiChannels[i].Name);
		MetaData.Color = MidiChannels[i].TrackColor;

		Channels.Add(MidiNoteChannels[i], MetaData, TMovieSceneExternalValue<int>());
	}
#else
	//no metadata in runtime
	for (int i = 0; i < MidiNoteChannels.Num(); i++)
	{
		Channels.Add(MidiNoteChannels[i]);
	}
#endif

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
		// if bMarkNoteOffs changed, rebuild keyframes
		if (PropertyName == GET_MEMBER_NAME_CHECKED(UBkMovieSceneMidiTrackSection, bMarkNoteOffs))
		{
			RebuildNoteKeyFrames();
		}

	}
}
#endif