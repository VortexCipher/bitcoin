# The mathematics of Minisketch sketches

This is an unconventional mathematical overview of the PinSketch algorithm without references to coding theory<sup>[[1]](#myfootnote1)</sup>.

## Set sketches

A sketch, for the purpose of this description, can be seen as a "set checksum" with two peculiar properties:

* Sketches have a predetermined capacity, and when the number of elements in the set is not higher than the capacity, minisketch will always recover the entire set from the sketch. A sketch of *b*-bit elements with capacity *c* can be stored in *bc* bits.
* The sketches of two sets can be combined by adding them (XOR) to obtain a sketch of the [symmetric difference](https://en.wikipedia.org/wiki/Symmetric_difference) between the two sets (*i.e.*, all elements that occur in one but not both input sets).

