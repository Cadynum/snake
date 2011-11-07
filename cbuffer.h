#include "types.h"

#ifndef CBUF_TYPE
#define CBUF_TYPE int
#endif

#define BUFSIZE 10

struct(cbuf)
{
	CBUF_TYPE value[10];
	size_t size;
	size_t spos;
	size_t epos;

};

static void cbuf_init(cbuf* c)
{
	c->size = 0;
	c->spos = 0;
	c->epos = 0;
}

static void cbuf_push(cbuf* c, CBUF_TYPE new)
{
	if( c->size < BUFSIZE ) {
		c->value[c->epos] = new;
		c->size++;
		c->epos = (c->epos + 1) % BUFSIZE;
	}
}

static CBUF_TYPE cbuf_pop(cbuf* c)
{
	if(c->size == 0)
		return 0;

	CBUF_TYPE ret = c->value[c->spos];
	c->spos = (c->spos+1) % BUFSIZE;
	c->size--;
	return ret;
}
