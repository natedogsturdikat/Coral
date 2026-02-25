# Coral
Coral is a four voice chorus and pitch shifter plugin made in C++ with JUCE. It is my University of Oregon Music Technology Senior Project

How Coral works:
Coral duplicates a signal into 4 copies, and modulates their pitch with individual LFOs per voice. Each voice has an independant gain, pan, and pitch control. Each voice's chorus effect is controlled by three master parameters with a wide range, Rate, Depth, and Dry/Wet

Rate: Basic, exponentially scaled rate knob in HZ

Depth: Controls the amplitude of each voice's LFO. Depth additionally controls the stereo spread of the chorus, or it's width. This stereo imaging is seperate from the voice pan controls, and simply controls the     desynchronization between the left and right channels

Dry/Wet: The dry signal is a simple mono or stereo passthrough of the original source audio. The wet signal is summed to mono if the input is stereo, with the stereo image controlled by the voice panning and the depth controls. This knob simply controls the balance.

Gain: Each voice has a gain parameter ranging from -60(treated as -inf) to +6dB, defaulting at 0. Because of the four voice boosting, there is a soft clipper added to the final stereo sum

Pan: Each voice has a pan parameter, which adjusts the gain automatically to reduce when in center (pan law)

Pitch: Each voice has a pitch control ranging from -12 to +12 semitones, with a toggleable button. this slider is stepwise(but you can adjust it in floats/microtones in DAW automation) *hint: if you can't find this one click the toggle button under the depth knob

Extras:
-Click the text label of any voice to mute it, this will turn the GUI elements for that voice grey until unmuted

Stereo In
  |
  +--> Dry (stereo) ---------------------------------------------\
  |                                                               \
  |                                                                \
  +--> Wet Input: Stereo → Mono                                     \
        |                                                            \
        v                                                             v
      Voice Processing (×4 parallel) --------------------------->  Dry/Wet Mix  --->  Soft Clip  --->  Out (stereo)
        |                                                          ^
        |                                                          |
        |   Pitch Shift?                                           |
        |            |                                             |
        |            v                                             |
        |         Chorus  <--- Rate / Depth (master)               |
        |            |                                             |
        |            v                                             |
        |         Width   <--- Depth (master)                      |
        |            |                                             |
        |            v                                             |
        |      Gain (dB) → Pan → Mute?                             |
        |                                                          |
        +--> Sum wetL / wetR---------------------------------------|
