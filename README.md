# Android test app: Seek to audio cue

This app plays a relatively long wav sound file (>4 minutes), and jumps the playback head to various cues. It does so in sync with a click. The test audio file is a looping countdown recorded at 60bpm, with a metronome sound, and an underlying tone, going up a semitone with each new cycle. What's being tested here, is that the app-synthesized click and the metronome sound from the audio file stay in sync whenever playback cues are requested. The cues are supposed to jump only on the beats (ie quantized).

I got lazy adding the test file to the app assets, or figuring out how to correctly load it, so instead, you need to copy the audio file ```countdown_tone_long.wav``` to your device (or emulator) external storage, within a "test" folder.
