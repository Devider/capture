/*
 * types.h
 *
 *  Created on: Aug 24, 2012
 *      Author: kirill
 */

#ifndef TYPES_H_
#define TYPES_H_
#define rgb_ptr unsigned char*
#define CLEAR(x) memset (&(x), 0, sizeof (x))

typedef struct _buffer {
	void * start;
	size_t length;
} buffer;
#endif
