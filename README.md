Status:
Epic are adding better integration between the movie sequencer and midi files, seems like it will be added to the 5.6 timeframe: https://github.com/EpicGames/UnrealEngine/commit/6145872aaa92ea6c9e50ed07adfd22fbf736e3f3

So probably this repository won't see much maintanenace from me in the future unless the Epic updates make it feasible to add actual midi editing support to the unreal sequencer, in the meanwhile I think this repo can serve as decent reference for adding custom tracks with slate widgets to the unreal sequencer. 

# UnrealMovieSequencerMidiUtils

Drag and drop Midi files (imported using the Harmonix plugin) into the sequencer to create Midi tracks that automatically create key frames on every note event from the midi file, useful for musically synced animations.

![Screenshot 2024-09-03 14-56-38](https://github.com/user-attachments/assets/d57d12a3-af19-42e5-87d9-cd030c3e1bb5)

Can also mark frames on musical grid units using the midi file's clock.

![Screenshot 2024-09-03 14-57-51](https://github.com/user-attachments/assets/47f8ce36-3a13-4a18-941a-32a9b7613f22)


![Screenshot 2024-09-03 14-58-53](https://github.com/user-attachments/assets/b9e7147f-153e-4971-93aa-a063d3bf6d18)

This plug in does not render midi or broadcast it to the scene.


# Community/Feedback/Support -  
 Discord  - https://discord.gg/hTKjSfcbEn
