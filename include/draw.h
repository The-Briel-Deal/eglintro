#ifndef GF_DRAW_H
#define GF_DRAW_H


#include <stdbool.h>

//! Draw a box in the window, `x` is the percentage across the screen the top
//! left of the box starts at. Width is the percentage size of the box relative
//! to screen (e.g. 1.00 means the box is the width of the screen, and 0.50
//! means the box's width is half the screen width. `y` and `height` follow the
//! same principles.
bool gf_draw_box(float x, float y, float width, float height);

#endif
