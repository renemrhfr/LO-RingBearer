# LO-RingBearer
This plugin does RingModulation from a Sidechain Signal on the desired Amplitude-Level. This way you can apply RingModulation only near the zero-crossings, only on the maximum amplitude or wherever in between you desire. This can be useful for keeping a clean transient and only decorating the quieter area of your sound.

![Screenshot](Screenshot.png)

# Parameters
You can set the Area to be effected with the `Low` and `High` Knobs.
The part of the Waveform that where the effect is applied, is indicated by a blue, grainy overlay.
The Waveform is a Snapshot of the Audio *BEFORE* the effect gets applied.

As Ringmodulation results in lower amplitude levels of the effected signal, you may need to adjust the uneffected Signal with
the `Dry Signal` Parameter to maintain the harmonic relationship of the Signal.

# Requirements
You need the JUCE Framework to compile this project (https://juce.com)