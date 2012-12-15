#ifndef PFORM_H_
#define PFORM_H_

#include <cmath>
#include <stdexcept>
#include <vector>
#include "entity.hpp"
#include "level.hpp"

#define PPB 16 // size of block in pixels, should be power of 2

#define CLAMP(l, x, u) x = ((x) < (l) ? (l) : ((x) > (u) ? (u) : (x)))
#define SIGN(x) ((x) > 0 ? 1 : ((x) < 0 ? -1 : 0))

#endif
