#pragma once


#include "CoreMinimal.h"

#include "Widgets/SWidget.h"
#include "ISequencerSection.h"
#include "MovieSceneTrack.h"
#include "ISequencer.h"
#include "ISequencerTrackEditor.h"
#include "MovieSceneTrackEditor.h"
#include "MovieSceneSection.h"
#include "HarmonixMidi/MidiFile.h"
#include "IContentBrowserSingleton.h"

#include "BkMovieSceneMidiTrack.h"
//#include "Bk"


class FMidiSceneSectionPainter : public ISequencerSection
{
public:
	FMidiSceneSectionPainter(UMovieSceneSection& InSection, TWeakPtr<ISequencer> InSequencer);

	virtual FText GetSectionTitle() const override;

	virtual float GetSectionHeight() const override;
	virtual void BeginResizeSection() override;
	virtual void ResizeSection(ESequencerSectionResizeMode ResizeMode, FFrameNumber ResizeTime) override;
	virtual void BeginSlipSection() override;
	virtual void SlipSection(FFrameNumber SlipTime) override;
	

	virtual int32 OnPaintSection(FSequencerSectionPainter& InPainter) const override;

	virtual UMovieSceneSection* GetSectionObject() override;

	/** The section we are visualizing. */
	UMovieSceneSection& Section;

	/** Cached start offset value valid only during resize */
	FFrameNumber InitialStartOffsetDuringResize;

	/** Cached start time valid only during resize */
	FFrameNumber InitialStartTimeDuringResize;
};

class FBkMovieSceneMidiTrackEditor : public FMovieSceneTrackEditor
{
	
	public:
	FBkMovieSceneMidiTrackEditor(TSharedRef<ISequencer> InSequencer);
	// ISequencerTrackEditor interface


	static TSharedRef<ISequencerTrackEditor> CreateTrackEditor(TSharedRef<ISequencer> InSequencer);


	virtual bool HandleAssetAdded(UObject* Asset, const FGuid& TargetObjectGuid) override;
	virtual bool SupportsType(TSubclassOf<UMovieSceneTrack> TrackClass) const override;
	virtual void BuildAddTrackMenu(FMenuBuilder& MenuBuilder) override;
	virtual TSharedPtr<SWidget> BuildOutlinerEditWidget(const FGuid& ObjectBinding, UMovieSceneTrack* Track, const FBuildEditWidgetParams& Params) override;
	virtual TSharedRef<ISequencerSection> MakeSectionInterface(UMovieSceneSection& SectionObject, UMovieSceneTrack& Track, FGuid ObjectBinding) override;
	virtual bool IsResizable(UMovieSceneTrack* InTrack) const override { return true; }
	virtual void Resize(float NewSize, UMovieSceneTrack* InTrack) override;

	//

private:

	/** Audio sub menu */
	TSharedRef<SWidget> BuildMidiSubmenu(FOnAssetSelected OnAssetSelected, FOnAssetEnterPressed OnAssetEnterPressed);

	/** Audio asset selected */
	void OnDawAssetSelected(const FAssetData& AssetData, UMovieSceneTrack* Track);

	/** Audio asset enter pressed */
	void OnDawAssetEnterPressed(const TArray<FAssetData>& AssetData, UMovieSceneTrack* Track);

	// The sequencer bound to this handler.  Used to access movie scene and time info during auto-key
protected:

	/** Delegate for AnimatablePropertyChanged in HandleAssetAdded for sounds */
	FKeyPropertyResult AddNewMidiFile(FFrameNumber KeyTime, class UMidiFile* InMidiFile, UBkMovieSceneMidiTrack* Track, int32 RowIndex);

	//TSharedPtr<ISequencer> Sequencer;
};