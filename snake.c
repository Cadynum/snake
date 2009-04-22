/*
	Snake, written by Christoffer Öjeling aka "Cadynum".

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "types.h"
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <math.h>
#include <assert.h>


#define VTYPE int
#include "v2.h"

#include "cbuffer.h"

#define RGB_BWMAP(x) ( (x) << 0x10 | (x) << 0x8 | (x) )

#define SNAKE_GROWTH		3
#define SNAKE_INIT_SIZE		3
#define SNAKE_DRAW_OFFSET	3
#define SNAKE_W				41
#define SNAKE_H				21


struct (speed_t) {
	const char* name;
	const uint value;
	SDL_Surface* txt;
};

struct (level_t) {
	const char* name;
	const char bg[SNAKE_H][SNAKE_W];
	SDL_Surface* txt;
};

uint speed = 1;
#define MAX_SPEED asizeof(speeds)
speed_t speeds[] = {
	{"javascript", 150, NULL},
	{"python", 100, NULL},
	{"c", 60, NULL},
	{"assembly", 40, NULL}
};

uint level = 0;
#define MAX_LEVEL asizeof(levels)
/* # = wall
 * A = Don't spawn apple here
 * [anything else] = blank
 */

level_t levels[] = {

	{"Lambda",
	{"####################-####################",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"-----------------------------------------",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"####################-####################"},
	NULL},

	{"Stoned",
	{"####################-####################",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"#--------#---------------------#--------#",
	"#--------#---------------------#--------#",
	"#--------#---------------------#--------#",
	"#--------#---------------------#--------#",
	"#--------#---------------------#--------#",
	"---------#---------------------#---------",
	"#--------#---------------------#--------#",
	"#--------#---------------------#--------#",
	"#--------#---------------------#--------#",
	"#--------#---------------------#--------#",
	"#--------#---------------------#--------#",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"####################-####################"},
	NULL},

	{"Diamond",
	{"####################-####################",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"#--------###########-###########--------#",
	"#--------###########-###########--------#",
	"#--------##########A-A##########--------#",
	"#--------#########AA-AA#########--------#",
	"#--------########AAA-AAA########--------#",
	"-----------------------------------------",
	"#--------########AAA-AAA########--------#",
	"#--------#########AA-AA#########--------#",
	"#--------##########A-A##########--------#",
	"#--------###########-###########--------#",
	"#--------###########-###########--------#",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"####################-####################"},
	NULL},

	{"Pointer",
	{"#########################################",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"######################------------------#",
	"---------------------#-------------------",
	"##############-------####################",
	"---------------------#-------------------",
	"######################------------------#",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"#---------------------------------------#",
	"#########################################"},
	NULL},

	{"Fooo", {""}, NULL}
};

enum direction
{
	NONE=0,
	UP,
	DOWN,
	LEFT,
	RIGHT
};

enum orentation
{
	CENTER,
	CENTERMIN,
	CENTERMAX,
	MIN,
	MAX
};


struct (snake)
{
	v2 bits[SNAKE_H * SNAKE_W];
	v2 head;
	enum direction velocity;
	uint bits_size;
	uint size;
	uint grow;
	bool alive;
	uint score;
};

//Globals
uint	bit_width = 20,
		bit_space = 3,
		bit_jump;

//Objects
v2 apple;
snake snake1;
cbuf inputs;

//Loop control
uint ticks;
uint timer;
bool loop = true;

//Assets
SDL_Surface * screen;
TTF_Font * font;
TTF_Font * fontmini;
SDL_Surface	*txt_score,
			*txt_snake,
			*txt_level,
			*txt_press,
			*txt_speedlvl;

SDL_Color fontcolor = {0xFF, 0xFF, 0x00, 0};

void (*state)();
void (*state_event)(SDL_Event*);
bool dim = true;


#include "snake.p"
int main()
{
	bit_jump = bit_space + bit_width;
	cbuf_init(&inputs);

	//Init SDL
	if( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == -1 || TTF_Init() == -1 ) {
		fprintf(stderr, "Init SDL: %s\n", SDL_GetError());
		exit(1);
	}
	SDL_WM_SetCaption("Snake", NULL);
	screen = SDL_SetVideoMode(SNAKE_W*bit_jump-bit_space,
		(SNAKE_H+SNAKE_DRAW_OFFSET) * bit_jump-bit_space,
		32, SDL_SWSURFACE);
	assert(screen);

	//Open fonts
 	font = TTF_OpenFont("font.ttf", bit_jump * 3);
 	fontmini = TTF_OpenFont("font.ttf", bit_jump * 2);
	assert(font);
	assert(fontmini);

	//Prerendered text
	txt_score	= TTF_RenderText_Blended(font,		"score: ",			fontcolor);
	txt_snake	= TTF_RenderText_Blended(font,		"cadynum's snake",		fontcolor);
	txt_press	= TTF_RenderText_Blended(fontmini, 	"press <space> to start",	fontcolor);
	txt_speedlvl	= TTF_RenderText_Blended(fontmini,	"speed: ",			fontcolor);
	txt_level	= TTF_RenderText_Blended(fontmini,	"level: ",			fontcolor);
	for (uint i=0; i<MAX_SPEED; i++) {
		speeds[i].txt = TTF_RenderText_Blended(fontmini, speeds[i].name, fontcolor);
	}
	for (uint i=0; i<MAX_LEVEL; i++) {
		levels[i].txt = TTF_RenderText_Blended(fontmini, levels[i].name, fontcolor);
	}

	//Default states
	state = state_start;
	state_event = state_start_event;

	ticks = 0;
	timer = SDL_GetTicks();
	while(loop) {
		events();
		grid_draw();
		state();
		SDL_Flip(screen);

		//Frame rate control
		uint now = SDL_GetTicks() - timer;
		if(now < speeds[speed].value)
			SDL_Delay( speeds[speed].value - now );
		now = SDL_GetTicks();
		ticks =  now - timer;
		timer = now;
		//printf("fps: %.1f : %d\n", 1000.0/ticks, ticks);
	}


	SDL_FreeSurface( txt_score );
	SDL_FreeSurface( txt_snake );
	SDL_FreeSurface( txt_level );
	SDL_FreeSurface( txt_press );
	for(uint i=0; i<MAX_SPEED; i++) {
		SDL_FreeSurface(speeds[i].txt);
	}
	for(uint i=0; i<MAX_LEVEL; i++) {
		SDL_FreeSurface(levels[i].txt);
	}
	TTF_CloseFont( font );
	TTF_CloseFont( fontmini );
	TTF_Quit();
	SDL_Quit();
	return 0;
}

void grid_draw()
{
	SDL_FillRect(screen, NULL, 0x000000);

	for(uint wh=0; wh < SNAKE_H; wh++)
	for(uint ww=0; ww < SNAKE_W; ww++) {
		glow_hline(screen, (wh+SNAKE_DRAW_OFFSET) * bit_jump - bit_space, bit_space, RGB_BWMAP(22));
		if(ww)
			glow_vline(screen, ww * bit_jump - bit_space, SNAKE_DRAW_OFFSET*bit_jump, screen->h, bit_space, RGB_BWMAP(22));
		if(levels[level].bg[wh][ww] == '#')
			blit(screen, (v2){ww, wh}, 0x00FF00);
	}
}

void state_start_event(SDL_Event * event)
{
	if(event->type == SDL_KEYDOWN) {
		switch(event->key.keysym.sym) {
			case SDLK_LEFT:
				if(level > 0) level--;
				break;

			case SDLK_RIGHT:
				if(level < MAX_LEVEL-1) level++;
				break;

			case SDLK_DOWN:
				if(speed > 0) speed--;
				break;

			case SDLK_UP:
				if(speed < (int)MAX_SPEED-1) speed++;
				break;

			case SDLK_SPACE:
				dim = false;
				state = state_play;
				state_event = state_play_event;
				snake_reset(&snake1);
				break;

			default: break;
		}
	}
}

void state_start()
{
	SDL_BlitSpecial(CENTER, 0, MIN, 0, txt_snake, screen);

	SDL_BlitSpecial(CENTERMIN, 0, CENTER, (SNAKE_DRAW_OFFSET-3)*bit_jump, txt_level, screen);
	SDL_BlitSpecial(CENTERMAX, 0, CENTER, (SNAKE_DRAW_OFFSET-3)*bit_jump, levels[level].txt, screen);

	SDL_BlitSpecial(CENTERMIN, 0, CENTER, (SNAKE_DRAW_OFFSET-1)*bit_jump, txt_speedlvl, screen);
	SDL_BlitSpecial(CENTERMAX, 0, CENTER, (SNAKE_DRAW_OFFSET-1)*bit_jump, speeds[speed].txt, screen);

	SDL_BlitSpecial(CENTER, 0, CENTER, (SNAKE_DRAW_OFFSET+5)*bit_jump, txt_press, screen);
}

void state_play()
{
	snake_move(&snake1, cbuf_pop(&inputs));

	if(!snake1.alive) {
		state_event = state_dead_event;
		dim = true;
	}

	snake_draw(&snake1, screen);
	apple_draw(&apple, screen);

	char txtbuf[1024];
	static uint score = 10000;
	static SDL_Surface * scoreval = NULL;
	if(score != snake1.score) {
		SDL_FreeSurface( scoreval );
		snprintf(txtbuf, sizeof txtbuf, "%u", snake1.score);
		scoreval = TTF_RenderText_Blended(font, txtbuf, fontcolor);
		score = snake1.score;
	}

	SDL_BlitSpecial(MAX, -bit_jump*5, MIN, 0, txt_score, screen);
	SDL_BlitSpecial(MAX, 0, MIN, 0, scoreval, screen);
}

void state_play_event(SDL_Event * event)
{
	if (event->type == SDL_KEYDOWN) {
		switch (event->key.keysym.sym) {
			case SDLK_UP:
				cbuf_push(&inputs, UP);
				break;

			case SDLK_DOWN:
				cbuf_push(&inputs, DOWN);
				break;

			case SDLK_LEFT:
				cbuf_push(&inputs, LEFT);
				break;

			case SDLK_RIGHT:
				cbuf_push(&inputs, RIGHT);
				break;

			default: break;
		}
	}
}

void state_dead_event(SDL_Event * event)
{
	switch(event->type) {
		case SDL_KEYDOWN:
			switch(event->key.keysym.sym) {
				case SDLK_SPACE:
					state = state_start;
					state_event = state_start_event;
				default: break;
			}
		default: break;
		}
}

void blit(SDL_Surface * screen, v2 co, u32 color)
{
	SDL_Rect rect = {
		.w = bit_width,
		.h = bit_width,
		.x = co.x * bit_jump,
		.y = (co.y+SNAKE_DRAW_OFFSET) * bit_jump
	};

	if (dim) {
		uchar r = ((color >> 0x10) & 0xFF) / 5;
		uchar g = (color >> 0x8 & 0xFF) / 5;
		uchar b = (color & 0xFF) / 5;
		color = r << 0x10 | g << 0x8 | b;
	}

	SDL_FillRect(screen, &rect, color);
}

static void events()
{
	SDL_Event event;

	while(SDL_PollEvent(&event)) {
		if(event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
			loop = false;
		}
		state_event(&event);
	}
}

void apple_rand(v2 *apple, snake * s)
{
	do {
		apple->x = rand()%SNAKE_W;
		apple->y = rand()%SNAKE_H;
	} while(v2_find(*apple, s->bits, s->bits+s->size) || levels[level].bg[apple->y][apple->x] == '#' || levels[level].bg[apple->y][apple->x] == 'A');
}


void apple_draw(v2 *apple, SDL_Surface *screen)
{
	blit(screen, *apple, 0xFF0000);
}


void snake_reset(snake * s)
{
	s->velocity = NONE;
	s->bits_size = sizeof s->bits;
	s->size = 1;
	s->grow = SNAKE_INIT_SIZE-1;
	s->head = s->bits[0] = (v2){SNAKE_W/2, SNAKE_H/2};
	s->alive = true;
	s->score = 0;
	apple_rand(&apple, s);
}


void snake_move(snake * s, enum direction velocity)
{
	if(!s->alive)
		return;

	switch(velocity) {
		case UP:
			if(s->velocity != DOWN) s->velocity = UP;
			break;

		case DOWN:
			if(s->velocity != UP) s->velocity = DOWN;
			break;

		case LEFT:
			if(s->velocity != RIGHT) s->velocity = LEFT;
			break;

		case RIGHT:
			if(s->velocity != LEFT) s->velocity = RIGHT;
			break;

		default: break;
	}

	switch(s->velocity) {
		case NONE:	return;
		case UP:	s->head.y--;	break;
		case DOWN:	s->head.y++;	break;
		case LEFT:	s->head.x--;	break;
		case RIGHT:	s->head.x++;	break;
	}

	if(s->head.x < 0) {
		s->head.x = SNAKE_W-1;
	} else if(s->head.x >= SNAKE_W) {
		s->head.x = 0;
	}

	if(s->head.y < 0) {
		s->head.y = SNAKE_H-1;
	} else if(s->head.y >= SNAKE_H) {
		s->head.y = 0;
	}


	if(v2_find(s->head, s->bits, s->bits+s->size-1)) {
		s->alive = false;
		return;
	}

	if(levels[level].bg[s->head.y][s->head.x] == '#') {
		s->alive = false;
		return;
	}

	if(V2_CMP(s->head, apple)) {
		s->score++;
		s->grow += SNAKE_GROWTH;
		apple_rand(&apple, s);
	}

	if(s->grow > 0) {
		s->bits[s->size] = s->bits[s->size-1];
		s->size++;
		s->grow--;
	}

	//Move all snake bits one step
	for(uint i=s->size-1; i > 0; i--) {
		s->bits[i] = s->bits[i-1];
	}
	s->bits[0] = s->head;




}


void snake_draw(snake * s, SDL_Surface * screen)
{
	for(uint i=0; i < s->size; i++) {
		blit(screen, s->bits[i], 0xFFFFFF);
	}
}

void glow_hline(SDL_Surface *dst, int pos, int size, u32 color)
{
	u32 *pixel = dst->pixels;
	int yjump = dst->pitch/4 - dst->w;

	pixel += dst->pitch/4  * pos;

	int x, y;
	for(y=0; y < size; y++, pixel+=yjump)
	for(x=0; x < dst->w; x++, pixel++) {
		*pixel = color;
	}
}


void glow_vline(SDL_Surface *dst, int xpos, int ystart, int yend, int size, u32 color)
{
	u32 (*pixel)[dst->pitch/4] = dst->pixels;

	int x, y;
	for(y=ystart; y < yend; y++)
	for(x=0; x < size; x++) {
		pixel[y][x+xpos] = color;
	}
}

void SDL_BlitSpecial(enum orentation xor, int xoff, enum orentation yor, int yoff, SDL_Surface *src, SDL_Surface *dst)
{
	SDL_Rect rect;

	switch(xor) {
		case CENTERMIN:	rect.x = dst->w/2 - src->w;	break;
		case CENTERMAX:	rect.x = dst->w/2;		break;
		case CENTER:	rect.x = dst->w/2 - src->w/2;	break;
		case MIN:	rect.x = 0;			break;
		case MAX:	rect.x = dst->w - src->w;	break;
	}
	rect.x += xoff;

	switch(yor) {
		case CENTERMIN:	rect.y = dst->h/2 - src->h;	break;
		case CENTERMAX:	rect.y = dst->h/2;		break;
		case CENTER:	rect.y = dst->h/2 - src->h/2;	break;
		case MIN:	rect.y = 0;			break;
		case MAX:	rect.y = dst->h - src->h;	break;
	}
	rect.y += yoff;

	SDL_BlitSurface(src, NULL, dst, &rect);
}
