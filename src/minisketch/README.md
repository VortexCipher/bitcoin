# Minisketch: a library for [BCH](https://en.wikipedia.org/wiki/BCH_code)-based set reconciliation
<img align="right" src="doc/minisketch-vs.png" />

`libminisketch` is an optimized standalone MIT-licensed library with C API for constructing and decoding *set sketches*, which can be used for compact set reconciliation and other applications.
It is an implementation of the PinSketch<sup>[[1]](#myfootnote1)</sup> algorithm. An explanation of the algorithm can be found [here](doc/math.md).

## Sketches for set reconciliation

Sketches, as produced by this library, can be seen as "set checksums" with two peculiar properties:
* Sketches have a predetermined capacity, and when the number of elements in the set is not higher than the capacity, `libminisketch` will always recover the entire set from the sketch. A sketch of *b*-bit elements with capacity *c* can be stored in *bc* bits.
* The sketches of two sets can be combined by adding them (XOR) to obtain a sketch of the [symmetric difference](https://en.wikipedia.org/wiki/Symmetric_difference) between the two sets (*i.e.*, all elements that occur in one but not both input sets).

This makes them appropriate for a very bandwidth-efficient set reconciliation protocol. If Alice and Bob each have a set of elements, and they suspect that the sets largely but not entirely overlap,
they can use the following protocol to let both parties learn all the elements:
* Alice and Bob both compute a sketch of their set elements.
* Alice sends her sketch to Bob.
* Bob combines the two sketches, and obtains a sketch of the symmetric difference.
* Bob tries to recover the elements from the difference sketch.
* Bob sends every element in the difference that he has to Alice.

This will always succeed when the size of the difference (elements that Alice has but Bob doesn't plus elements that Bob has but Alice doesn't) does not exceed the
