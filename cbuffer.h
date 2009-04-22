#include "types.h"

#ifndef CBUF_TYPE
#define CBUF_TYPE int
#endif

struct(cbuf)
{
	CBUF_TYPE	value[10];
	uint size;
	uint spos;
	uint epos;

};

static void cbuf_init(cbuf* c)
{
	c->size = 0;

	c->spos = 0;
	c->epos = 0;
}

static void cbuf_push(cbuf* c, CBUF_TYPE new)
{
	if( c->size < asizeof(c->value) )
	{
		c->value[c->epos] = new;
		c->size++;
		if(c->epos == asizeof(c->value)-1)
			c->epos = 0;
		else
			c->epos++;
	}
}

static CBUF_TYPE cbuf_pop(cbuf* c)
{
	if(c->size > 0)
	{
		uint oldspos = c->spos;
		if(c->spos == asizeof(c->value)-1)
			c->spos = 0;
		else
			c->spos++;
		c->size--;
		return c->value[oldspos];
	}
	return 0;
}

static CBUF_TYPE cbuf_peek(cbuf* c)
{
	if(c->size > 0)
		return c->value[c->spos];
	else
		return 0;
}
