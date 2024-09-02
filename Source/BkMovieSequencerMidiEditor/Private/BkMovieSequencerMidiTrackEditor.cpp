#include "BkMovieSequencerMidiTrackEditor.h"
#include "SequencerSectionPainter.h"
#include "EditorStyleSet.h"
#include "TimeToPixel.h"
#include "ISequencer.h"

#include "BkMovieSceneMidiTrackSection.h"


FBkMovieSceneMidiTrackEditor::FBkMovieSceneMidiTrackEditor(TSharedRef<ISequencer> InSequencer) : FMovieSceneTrackEditor(InSequencer)
{
}

TSharedRef<ISequencerTrackEditor> FBkMovieSceneMidiTrackEditor::CreateTrackEditor(TSharedRef<ISequencer> InSequencer)
{

	return MakeShareable(new FBkMovieSceneMidiTrackEditor(InSequencer));
}

bool FBkMovieSceneMidiTrackEditor::HandleAssetAdded(UObject* Asset, const FGuid& TargetObjectGuid)
{

	if (Asset->IsA<UMidiFile>())
	{
		UE_LOG(LogTemp, Warning, TEXT("HandleAssetAdded MidiFile"));
		UMidiFile* MidiFileFromAsset = Cast<UMidiFile>(Asset);
		UBkMovieSceneMidiTrack* DummyTrack = nullptr;
		TArray<TWeakObjectPtr<>> OutObjects;

		const FScopedTransaction Transaction(FText::FromString("Add Daw Session"));

		int32 RowIndex = INDEX_NONE;
		AnimatablePropertyChanged(FOnKeyProperty::CreateRaw(this, &FBkMovieSceneMidiTrackEditor::AddNewMidiFile, MidiFileFromAsset, DummyTrack, RowIndex));
		
		return true;
	}

	return false;
}
bool FBkMovieSceneMidiTrackEditor::SupportsType(TSubclassOf<UMovieSceneTrack> TrackClass) const
{
	return UBkMovieSceneMidiTrack::StaticClass() == TrackClass;
}
void FBkMovieSceneMidiTrackEditor::BuildAddTrackMenu(FMenuBuilder& MenuBuilder)
{

}
TSharedPtr<SWidget> FBkMovieSceneMidiTrackEditor::BuildOutlinerEditWidget(const FGuid& ObjectBinding, UMovieSceneTrack* Track, const FBuildEditWidgetParams& Params)
{
	return SNullWidget::NullWidget;
}
TSharedRef<ISequencerSection> FBkMovieSceneMidiTrackEditor::MakeSectionInterface(UMovieSceneSection& SectionObject, UMovieSceneTrack& Track, FGuid ObjectBinding)
{
	return MakeShareable(new FMidiSceneSectionPainter(SectionObject, GetSequencer()));
}

void FBkMovieSceneMidiTrackEditor::Resize(float NewSize, UMovieSceneTrack* InTrack)
{
	
	auto Track = Cast<UBkMovieSceneMidiTrack>(InTrack);
	if (Track)
	{
		for (auto Section : Track->GetAllSections())
		{
			auto MidiSection = Cast<UBkMovieSceneMidiTrackSection>(Section);
			if (MidiSection)
			{
				MidiSection->SectionHeight = FMath::Max(NewSize, 25.0f);
			}
		}
	}
}

TSharedRef<SWidget> FBkMovieSceneMidiTrackEditor::BuildMidiSubmenu(FOnAssetSelected OnAssetSelected, FOnAssetEnterPressed OnAssetEnterPressed)
{
	return SNullWidget::NullWidget;
}

void FBkMovieSceneMidiTrackEditor::OnDawAssetSelected(const FAssetData& AssetData, UMovieSceneTrack* Track)
{
}

void FBkMovieSceneMidiTrackEditor::OnDawAssetEnterPressed(const TArray<FAssetData>& AssetData, UMovieSceneTrack* Track)
{
}

FKeyPropertyResult FBkMovieSceneMidiTrackEditor::AddNewMidiFile(FFrameNumber KeyTime, UMidiFile* InMidiFile, UBkMovieSceneMidiTrack* Track, int32 RowIndex)
{
	
	FKeyPropertyResult KeyPropertyResult;

	UMovieScene* FocusedMovieScene = GetFocusedMovieScene();
	if (FocusedMovieScene->IsReadOnly())
	{
		return KeyPropertyResult;
	}

	FFindOrCreateRootTrackResult<UBkMovieSceneMidiTrack > TrackResult;// = FindOrCreateRootTrack<UUndawSequenceMovieSceneTrack>(FocusedMovieScene);

	TrackResult.Track = Track;

	if (!Track)
	{

		Track = FocusedMovieScene->AddTrack<UBkMovieSceneMidiTrack>();
		//TODO: Check if midi file already exists, maybe? although it is perfectly acceptable for the user to add the same midi file to multiple tracks

	}

	if (ensure(Track))
	{

		Track->Modify();
		Track->SetDisplayName(FText::FromName(InMidiFile->GetFName()));

		auto* NewSection = Track->AddNewMidiTrackOnRow(KeyTime, RowIndex, InMidiFile);

		KeyPropertyResult.SectionsCreated.Add(NewSection);


		GetSequencer()->OnAddTrack(Track, FGuid());
		
	}


	KeyPropertyResult.bTrackModified = true;
		

	

	
	return KeyPropertyResult;
}
;


FMidiSceneSectionPainter::FMidiSceneSectionPainter(UMovieSceneSection& InSection, TWeakPtr<ISequencer> InSequencer) : Section(InSection), Sequencer(InSequencer)
{
	MidiSection = Cast<UBkMovieSceneMidiTrackSection>(&InSection);
	BindToKeyModifiedDelegates();
}

FText FMidiSceneSectionPainter::GetSectionTitle() const
{

	
	return MidiSection->GetSectionTitle();
}

float FMidiSceneSectionPainter::GetSectionHeight() const
{
	return MidiSection->SectionHeight;
}



int32 FMidiSceneSectionPainter::OnPaintSection(FSequencerSectionPainter& InPainter) const
{

	InPainter.PaintSectionBackground();

	
	
	const FTimeToPixel& TimeToPixelConverter = InPainter.GetTimeConverter();
	FFrameRate TickResolution = TimeToPixelConverter.GetTickResolution();
	//auto UDawSection = Cast<UBkMovieSceneMidiTrackSection>(&Section);
	const float SectionStartTime = TickResolution.AsSeconds(MidiSection->GetInclusiveStartFrame());
	
	const auto& MidiSongsMap = MidiSection->Midi->GetSongMaps();
	const int NoteRange = MidiSection->MaxNotePitch - MidiSection->MinNotePitch + 3;
	for (int i = 0; i < MidiSection->MidiChannels.Num(); i++)
	{

		const auto& MidiTrack = MidiSection->MidiChannels[i].Notes;
		const auto& TrackColor = MidiSection->MidiChannels[i].TrackColor;
		if (MidiSection->MidiChannels[i].bIsVisible == false)
		{
			continue;
		}
		for (const auto& Note : MidiTrack)
		{
			//a bit ugly and magic numbers but we just want the notes to be offset from the very edges of the section by one note height
			const float MappedPitch = GetSectionHeight() / NoteRange * (NoteRange - ((Note.NoteNumber + 1) - MidiSection->MinNotePitch));
			const float NoteStartTime = MidiSongsMap->TickToMs(Note.StartTick);
			const float NoteEndTime = MidiSongsMap->TickToMs(Note.EndTick);
			const float NoteOffset = (NoteStartTime * .001f);
			float StartPixel = TimeToPixelConverter.SecondsToPixel(NoteStartTime * .001 + +SectionStartTime);
			float EndPixel = TimeToPixelConverter.SecondsToPixel(NoteEndTime * .001f + +SectionStartTime);
			//draw line a line from start time to end time at pitch height
			FSlateDrawElement::MakeLines(InPainter.DrawElements, InPainter.LayerId, InPainter.SectionGeometry.ToPaintGeometry(), TArray<FVector2D>{FVector2D(StartPixel, MappedPitch), FVector2D(EndPixel, MappedPitch)}, ESlateDrawEffect::None, TrackColor, false);
		}
	}

	

	
	return InPainter.LayerId;
}

UMovieSceneSection* FMidiSceneSectionPainter::GetSectionObject()
{
	return &Section;
}

void FMidiSceneSectionPainter::BindToKeyModifiedDelegates()
{

	Sequencer.Pin()->OnChannelChanged().AddLambda([this](const FMovieSceneChannelMetaData* MetaData, UMovieSceneSection* Section)
		{
			//print channel name
			/*for (auto& MidiChannel : MidiSection->MidiNoteChannels)
			{
				if (MidiChannel.GetData(). == Channel)
				{
					UE_LOG(LogTemp, Warning, TEXT("Found a midi channel?"));
				}
			}*/
			if (Section->StaticClass() == UBkMovieSceneMidiTrackSection::StaticClass()) MidiSection->RebuildNoteKeyFrames();

		});
	//MidiSection->On
	//for (auto& MidiChannel : MidiSection->MidiNoteChannels)
	//{
	//	if (!MidiChannel.OnKeyMovedEvent().IsBound())
	//	{
	//		MidiChannel.OnKeyMovedEvent().AddLambda([this](FMovieSceneChannel* Channel, const  TArray<FKeyMoveEventItem>& MovedItems)
	//			{
	//				//UE_LOG(LogTemp, Warning, TEXT("OnKeyMovedEvent"));
	//				if (!MidiSection->IsRebuildingKeys()) MidiSection->RebuildNoteKeyFrames();
	//			});

	//	}
	//	
	//	if (!MidiChannel.OnKeyAddedEvent().IsBound())
	//	{
	//		MidiChannel.OnKeyAddedEvent().AddLambda([this](FMovieSceneChannel* Channel, const  TArray<FKeyAddOrDeleteEventItem>& Items)
	//			{
	//				if (!MidiSection->IsRebuildingKeys()) MidiSection->RebuildNoteKeyFrames();
	//			});
	//	}
	//	if (!MidiChannel.OnKeyDeletedEvent().IsBound())
	//	{
	//		MidiChannel.OnKeyDeletedEvent().AddLambda([this](FMovieSceneChannel* Channel, const  TArray<FKeyAddOrDeleteEventItem>& Items)
	//			{
	//				if (!MidiSection->IsRebuildingKeys()) MidiSection->RebuildNoteKeyFrames();
	//			});
	//	}
	//


};


