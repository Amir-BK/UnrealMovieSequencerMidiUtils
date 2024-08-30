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
		Track->ParseRawMidiEventsIntoNotesAndTracks(InMidiFile);

		for (auto& [Index, MidiTrack] : Track->MidiTracks)
		{
			auto* NewSection = Track->AddNewMidiTrackOnRow(MidiTrack, KeyTime, Index, InMidiFile);
			Track->SetTrackRowDisplayName(FText::FromName(MidiTrack.TrackName), Index);
			NewSection->NoteColor = FLinearColor::MakeRandomSeededColor(Index);
			KeyPropertyResult.SectionsCreated.Add(NewSection);

		}
		GetSequencer()->OnAddTrack(Track, FGuid());
		
	}


		KeyPropertyResult.bTrackModified = true;
		

	

	
	return KeyPropertyResult;
}
;

FMidiSceneSectionPainter::FMidiSceneSectionPainter(UMovieSceneSection& InSection, TWeakPtr<ISequencer> InSequencer)
	: Section(InSection)
{

}

FText FMidiSceneSectionPainter::GetSectionTitle() const
{
	auto UDawSection = Cast<UBkMovieSceneMidiTrackSection>(&Section);
	//Section.Get

	
	return UDawSection->GetSectionTitle();
}

float FMidiSceneSectionPainter::GetSectionHeight() const
{
	return 150.0f;
}

int32 FMidiSceneSectionPainter::OnPaintSection(FSequencerSectionPainter& InPainter) const
{

	InPainter.PaintSectionBackground();
	
	const FTimeToPixel& TimeToPixelConverter = InPainter.GetTimeConverter();
	FFrameRate TickResolution = TimeToPixelConverter.GetTickResolution();
	auto UDawSection = Cast<UBkMovieSceneMidiTrackSection>(&Section);
	const float SectionStartTime = TickResolution.AsSeconds(UDawSection->GetInclusiveStartFrame());
	
	const auto& MidiSongsMap = UDawSection->Midi->GetSongMaps();

		for (const auto& Note : UDawSection->MidiData.Notes)
		{
			const float NoteStartTime = MidiSongsMap->TickToMs(Note.StartTick);
			const float NoteEndTime = MidiSongsMap->TickToMs(Note.EndTick);
			const float NoteOffset = (NoteStartTime * .001f);
			float StartPixel = TimeToPixelConverter.SecondsToPixel(NoteStartTime * .001 + +SectionStartTime);
			float EndPixel = TimeToPixelConverter.SecondsToPixel(NoteEndTime * .001f + +SectionStartTime);
			//draw line a line from start time to end time at pitch height
			FSlateDrawElement::MakeLines(InPainter.DrawElements, InPainter.LayerId, InPainter.SectionGeometry.ToPaintGeometry(), TArray<FVector2D>{FVector2D(StartPixel, 127 - Note.NoteNumber), FVector2D(EndPixel, 127 - Note.NoteNumber)}, ESlateDrawEffect::None, UDawSection->NoteColor, false);
		}
	

	
	return InPainter.LayerId;
}

UMovieSceneSection* FMidiSceneSectionPainter::GetSectionObject()
{
	return &Section;
}

FMidiSceneConductorSectionPainter::FMidiSceneConductorSectionPainter(UMovieSceneSection& InSection, TWeakPtr<ISequencer> InSequencer) : Section(InSection)
{
	TimeSlider = InSequencer.Pin()->GetTopTimeSliderWidget();
}

FText FMidiSceneConductorSectionPainter::GetSectionTitle() const
{
	return FText::GetEmpty();
}

float FMidiSceneConductorSectionPainter::GetSectionHeight() const
{
	return 50.0f;
}

int32 FMidiSceneConductorSectionPainter::OnPaintSection(FSequencerSectionPainter& InPainter) const
{
	FSlateDrawElement::MakeBox(InPainter.DrawElements, InPainter.LayerId, InPainter.SectionGeometry.ToPaintGeometry(), FAppStyle::GetBrush("Sequencer.Section.Background"), ESlateDrawEffect::None, FLinearColor::White);
	//draw text saying "conductor"

	FSlateDrawElement::MakeText(InPainter.DrawElements, InPainter.LayerId, InPainter.SectionGeometry.ToPaintGeometry(), FText::FromString("Conductor"), FAppStyle::GetFontStyle("NormalFont"), ESlateDrawEffect::None, FLinearColor::White);
	
	return int32();
}

UMovieSceneSection* FMidiSceneConductorSectionPainter::GetSectionObject()
{
	return &Section;
}
