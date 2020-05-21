# MIDI-controlled sampler and sequencer for the Bela platform

MIDI-controlled real-time audio sampler with looping and sequencing capabilities, implemented on the [Bela](bela.io) platform. It has a 16-step built-in sequencer and can be used as a looping station. It comes with a simple effects unit (tremolo, low-pass filter, delay) with parameters controlled through the MIDI device.

The MIDI interface in this implementation follows the design of the Novation Launchkey Mini MIDI controller keyboard, but can be easily remapped to most other MIDI devices. The MIDI messages table on the manual, which can be found [here](https://fael-downloads-prod.focusrite.com/customer/prod/s3fs-public/novation/downloads/6945/launchkey-mini-ug-en.pdf), is a useful reference for this.

![](https://github.com/ilaria-manco/bela-sampler/blob/master/diagram.jpg)
