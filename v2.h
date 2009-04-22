#include "types.h"

#ifndef VTYPE
#define VTYPE int
#endif


struct(v2)
{
	VTYPE x, y;
};

#define V2_CMP(v1, v2) ((v1).x == (v2).x && (v1).y == (v2).y)

static inline bool v2_find(v2 find, v2 * first, v2 * last)
{
	v2 * ptr;

	for(ptr = first; ptr != last; ptr++) {
		if(V2_CMP(*ptr, find)) {
			return true;
		}
	}
	return false;
}
