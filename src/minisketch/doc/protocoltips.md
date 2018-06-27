# Tips for designing protocols using `libminisketch`

Sending a sketch is less efficient than just sending your whole set with efficient entropy coding if the number of differences is larger than *log<sub>2</sub>( 2<sup>b</sup> choose set_size ) / b*.
