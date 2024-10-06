# Modelisme library

This library ease the use of multiple channel in a R/C model and only 1 receiver.
This library will decode all channels from SBUS signal.
This library can also manage ligths (lights, brake, turn light, ...).
And finally all those informations can be displayed on a small screen within the model.

# connections

SBus signal must be inverted, and connected to port 1 of arduino, on the hardware serial RX pin.
The easy way to connect lights is to use an ULN2003a and connect it to pins 2->8.
