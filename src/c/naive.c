#include <pebble.h>

#define CONFKEY	1
#define MARGIN	3
#define SPACING	2
#define FONT0H	7
#define FONT1H	56
#define FONT2H	84
#define FONT12W	60
#define SHADOWW	14
#define BLOBN	20

enum icon {
	ICON_HEART = 1,		/* Skip 0, it's used as null terminator */
	ICON_QUIET,
	ICON_CHARGING,
	ICON_BATTERY_0,
	ICON_BATTERY_20,
	ICON_BATTERY_40,
	ICON_BATTERY_60,
	ICON_BATTERY_80,
	ICON_BATTERY_100,
	ICON_WARNING,
	ICON_STEP,
	ICON_BLOB_0,
	ICON_BLOB_1,
	ICON_BLOB_2,
	ICON_BLOB_3,
	ICON_BLOB_4,
	ICON_BLOB_5,
	ICON_BLOB_6,
	ICON_BLOB_7,
	ICON_BLOB_8,
	ICON_BLOB_9,
	ICON_BLOB_10,
	ICON_BLOB_11,
	ICON_BLOB_12,
	ICON_BLOB_13,
	ICON_BLOB_14,
	ICON_BLOB_15,
	ICON_BLOB_16,
	ICON_BLOB_17,
	ICON_BLOB_18,
	ICON_BLOB_19,
	ICON_FLIPX = 127,	/* Special icon, flip horizontally next icon */
};

enum vibe {
	VIBE_SILENT,
	VIBE_SHORT,
	VIBE_LONG,
	VIBE_DOUBLE,
};

enum align {
	ALIGN_LEFT,
	ALIGN_RIGHT,
	ALIGN_CENTER,
};

typedef int8_t		i8;
typedef int16_t		i16;
typedef int32_t		i32;
typedef uint8_t		u8;
typedef uint16_t	u16;
typedef uint32_t	u32;
typedef enum vibe	Vibe;
typedef enum align	Align;
typedef struct tm	Tm;

static void	configure	();
static Tm*	now		();
static void	vibe		(Vibe);
static char*	formatstr	(char*);
static GRect	glyphrect	(char);
static void	drawpixel	(GBitmapDataRowInfo, i16, GColor);
static void	dither		(Layer*, GContext*, u8);
static void	onload		(Window*);
static void	onunload	(Window*);
static void	ontick		(struct tm*, TimeUnits);
static void	onconf		(DictionaryIterator*, void*);
static void	onconnection	(bool);
static void	onbattery	(BatteryChargeState);
#ifdef PBL_HEALTH
static void	onhealth	(HealthEventType, void*);
#endif
static void	ontap		(AccelAxisType, i32);
static void	ontimer		(void*);
static void	onbody		(Layer*, GContext*);
static void	ontext		(Layer*, GContext*, char*, Align);
static void	ontext0		(Layer*, GContext*);
static void	ontext1		(Layer*, GContext*);
static void	ontext2		(Layer*, GContext*);
static void	ontext3		(Layer*, GContext*);
static void	onhour0		(Layer*, GContext*);
static void	onhour1		(Layer*, GContext*);
static void	onhour2		(Layer*, GContext*);
static void	onminute0	(Layer*, GContext*);
static void	onminute1	(Layer*, GContext*);
static void	onminute2	(Layer*, GContext*);

static struct {
	GColor	bg;
	GColor	fg;
	char	text0[32];
	char	text1[32];
	char	text2[32];
	char	text3[32];
	Vibe	bton;
	Vibe	btoff;
	Vibe	hour;
	i8	seconds;
} conf;

static Layer*		body;
static Layer*		text0;
static Layer*		text1;
static Layer*		text2;
static Layer*		text3;
static Layer*		hour0;
static Layer*		hour1;
static Layer*		hour2;
static Layer*		minute0;
static Layer*		minute1;
static Layer*		minute2;
static GBitmap*		font0;
static GDrawCommandImage*	font1;
static GDrawCommandImage*	font2;
static u8		battery;
static bool		charging;
static HealthValue	steps;
static bool		connected;
static u8		blobi;

static struct { u16 x,w; } glyphs[128] = {
	/* 0 Index is never used as 0 is a null terminator in strings */
	[0] = {0,0},
	/* Use first whitespace ACSII characters for icons */
	[ICON_HEART]		= {420,9},
	[ICON_QUIET]		= {429,8},
	[ICON_CHARGING]		= {437,6},
	[ICON_BATTERY_0]	= {443,5},
	[ICON_BATTERY_20]	= {448,5},
	[ICON_BATTERY_40]	= {453,5},
	[ICON_BATTERY_60]	= {458,5},
	[ICON_BATTERY_80]	= {463,5},
	[ICON_BATTERY_100]	= {468,5},
	[ICON_WARNING]		= {473,7},
	[ICON_STEP]		= {481,13},
	[ICON_BLOB_0]		= {548,9},
	[ICON_BLOB_1]		= {557,9},
	[ICON_BLOB_2]		= {566,9},
	[ICON_BLOB_3]		= {575,9},
	[ICON_BLOB_4]		= {584,9},
	[ICON_BLOB_5]		= {593,9},
	[ICON_BLOB_6]		= {602,9},
	[ICON_BLOB_7]		= {611,9},
	[ICON_BLOB_8]		= {620,9},
	[ICON_BLOB_9]		= {629,9},
	[ICON_BLOB_10]		= {638,9},
	[ICON_BLOB_11]		= {647,9},
	[ICON_BLOB_12]		= {656,9},
	[ICON_BLOB_13]		= {665,11},
	[ICON_BLOB_14]		= {676,14},
	[ICON_BLOB_15]		= {690,15},
	[ICON_BLOB_16]		= {705,15},
	[ICON_BLOB_17]		= {720,17},
	[ICON_BLOB_18]		= {737,18},
	[ICON_BLOB_19]		= {755,13},
	/* Regular characters */
	[' ']={  0,6}, ['!']={  6,3}, ['"']={  9,4}, ['#']={ 13,6},
	['$']={ 19,6}, ['%']={ 25,7}, ['&']={ 32,7}, ['\'']={39,2},
	['(']={ 41,4}, [')']={ 45,4}, ['*']={ 49,7}, ['+']={ 56,6},
	[',']={ 62,4}, ['-']={ 66,5}, ['.']={ 71,3}, ['/']={ 74,5},
	['0']={ 79,7}, ['1']={ 86,7}, ['2']={ 93,7}, ['3']={100,7},
	['4']={107,7}, ['5']={114,7}, ['6']={121,7}, ['7']={128,7},
	['8']={135,7}, ['9']={142,7}, [':']={149,3}, [';']={152,4},
	['<']={156,6}, ['=']={162,5}, ['>']={167,6}, ['?']={173,7},
	['@']={180,7}, ['A']={187,7}, ['B']={194,7}, ['C']={201,7},
	['D']={208,7}, ['E']={215,7}, ['F']={222,7}, ['G']={229,7},
	['H']={236,7}, ['I']={243,3}, ['J']={246,7}, ['K']={253,7},
	['L']={260,6}, ['M']={266,8}, ['N']={274,7}, ['O']={281,7},
	['P']={288,7}, ['Q']={295,7}, ['R']={302,7}, ['S']={309,7},
	['T']={316,7}, ['U']={323,7}, ['V']={330,7}, ['W']={337,8},
	['X']={345,7}, ['Y']={352,7}, ['Z']={359,7}, ['[']={366,5},
	['\\']={371,5},[']']={376,5}, ['^']={381,7}, ['_']={388,6},
	['`']={394,4}, ['a']={187,7}, ['b']={194,7}, ['c']={201,7},
	['d']={208,7}, ['e']={215,7}, ['f']={222,7}, ['g']={229,7},
	['h']={236,7}, ['i']={243,3}, ['j']={246,7}, ['k']={253,7},
	['l']={260,6}, ['m']={266,8}, ['n']={274,7}, ['o']={281,7},
	['p']={288,7}, ['q']={295,7}, ['r']={302,7}, ['s']={309,7},
	['t']={316,7}, ['u']={323,7}, ['v']={330,7}, ['w']={337,8},
	['x']={345,7}, ['y']={352,7}, ['z']={359,7}, ['{']={398,6},
	['|']={404,3}, ['}']={407,6}, ['~']={413,7},
	/* Special */
	[ICON_FLIPX]={0,0}
};

void
configure()
{
#ifdef PBL_HEALTH
	if (strstr(conf.text0, "#s") ||
	    strstr(conf.text1, "#s") ||
	    strstr(conf.text2, "#s") ||
	    strstr(conf.text3, "#s")) {
		health_service_events_subscribe(onhealth, 0);
		onhealth(HealthEventSignificantUpdate, 0);
	} else {
		health_service_events_unsubscribe();
	}
#endif

	tick_timer_service_subscribe(conf.seconds == -1 ? SECOND_UNIT : MINUTE_UNIT, ontick);

	layer_mark_dirty(body);
}

Tm*
now()
{
	time_t timestamp;

	timestamp = time(0);
	return localtime(&timestamp);
}

void
vibe(Vibe type)
{
	switch (type) {
	case VIBE_SILENT: break;
	case VIBE_SHORT:  vibes_short_pulse();  break;
	case VIBE_LONG:   vibes_long_pulse();   break;
	case VIBE_DOUBLE: vibes_double_pulse(); break;
	}
}

char*
formatstr(char *fmt)
{
	static char buf[32];
	u32 i;

	for (i=0; *fmt && i < sizeof buf - 1; fmt++) {
		switch (*fmt) {
		case '#':		/* special values */
			fmt++;
			switch (*fmt) {
			case '#':
				buf[i++] = '#';
				break;
			case 'b':
			case 'B':
				i += snprintf(buf+i, sizeof buf - i, "%u", battery);
				break;
			case 's':
			case 'S':
				i += snprintf(buf+i, sizeof buf - i, "%ld", steps);
				break;
			case 'i':
			case 'I':
				i += snprintf(buf+i, sizeof buf - i, "%u", blobi);
				break;
			}
			break;
		case '*':		/* icons */
			fmt++;
			switch (*fmt) {
			case '*':
				buf[i++] = '*';
				break;
			case 'h':
			case 'H':
				buf[i++] = ICON_HEART;
				break;
			case 'q':
			case 'Q':
				if (quiet_time_is_active())
					buf[i++] = ICON_QUIET;
				break;
			case 'c':
			case 'C':
				if (charging)
					buf[i++] = ICON_CHARGING;
				break;
			case 'b':
			case 'B':
				/**/ if (battery > 95) buf[i++] = ICON_BATTERY_100;
				else if (battery > 75) buf[i++] = ICON_BATTERY_80;
				else if (battery > 55) buf[i++] = ICON_BATTERY_60;
				else if (battery > 35) buf[i++] = ICON_BATTERY_40;
				else if (battery > 15) buf[i++] = ICON_BATTERY_20;
				else                   buf[i++] = ICON_BATTERY_0;
				break;
			case 'w':
			case 'W':
				if (!connected)
					buf[i++] = ICON_WARNING;
				break;
			case 's':
			case 'S':
				buf[i++] = ICON_STEP;
				break;
			case 'l':
			case 'L':
				buf[i++] = ICON_BLOB_0 + blobi;
				break;
			case 'r':
			case 'R':
				buf[i++] = ICON_FLIPX;
				buf[i++] = ICON_BLOB_0 + blobi;
				break;
			}
			break;
		default:
			buf[i++] = *fmt;
		}
	}
	buf[i] = 0;
	return buf;
}

GRect
glyphrect(char c)
{
	GRect rect;

	rect.origin.x = glyphs[(int)c].x;
	rect.origin.y = 0;
	rect.size.w = glyphs[(int)c].w;
	rect.size.h = rect.size.w ? FONT0H : 0;

	return rect;
}

void
drawpixel(GBitmapDataRowInfo info, i16 x, GColor color)
{
#if defined(PBL_COLOR)
	memset(info.data + x, color.argb, 1);
#elif defined(PBL_BW)
	u8 byte  = x / 8;
	u8 bit   = x % 8;
	u8 value = gcolor_equal(color, GColorWhite) ? 1 : 0;
	u8 *bp   = &info.data[byte];
	*bp ^= (-value ^ *bp) & (1 << bit);
#endif
}

void
dither(Layer *layer, GContext *ctx, u8 amount)
{
	static const uint8_t map[8][8] = {
		{   0, 128,  32, 160,   8, 136,  40, 168 },
		{ 192,  64, 224,  96, 200,  72, 232, 104 },
		{  48, 176,  16, 144,  56, 184,  24, 152 },
		{ 240, 112, 208,  80, 248, 120, 216,  88 },
		{  12, 140,  44, 172,   4, 132,  36, 164 },
		{ 204,  76, 236, 108, 196,  68, 228, 100 },
		{  60, 188,  28, 156,  52, 180,  20, 148 },
		{ 252, 124, 220,  92, 244, 116, 212,  84 }
	};
	GRect rect;
	GBitmap *fb;
	GBitmapDataRowInfo info;
	i16 x, y, maxx, maxy;

	rect = layer_get_frame(layer);
	maxy = rect.origin.y + rect.size.h;
	fb = graphics_capture_frame_buffer(ctx);

	for (y = rect.origin.y; y < maxy; y++) {
		info = gbitmap_get_data_row_info(fb, y);
		maxx = rect.origin.x + rect.size.w;

		if (info.max_x < maxx)
			maxx = info.max_x;

		for (x = rect.origin.x; x < maxx; x++)
			if (amount > map[y%8][x%8])
				drawpixel(info, x, conf.bg);
	}
}

void
onload(Window *win)
{
	Layer *root;
	GRect rect;

	root = window_get_root_layer(win);
	rect = layer_get_bounds(root);

	body = layer_create(rect);
	layer_set_update_proc(body, onbody);
	layer_add_child(root, body);

	/*
	 * NOTE(irek): Text layers overlap on rectangle screens
	 * because text0 can take whole width when text1 is empty,
	 * or text2 it can take more than half if text3 very short.
	 */

	rect.origin.x = MARGIN;
	rect.origin.y = MARGIN;
	rect.size.w = PBL_DISPLAY_WIDTH - MARGIN*2;
	rect.size.h = FONT0H;

	text0 = layer_create(rect);
	layer_set_update_proc(text0, ontext0);
	layer_add_child(body, text0);

	text1 = layer_create(rect);
	layer_set_update_proc(text1, ontext1);
	layer_add_child(body, text1);

	rect.origin.y = PBL_DISPLAY_HEIGHT - MARGIN - FONT0H;

	text2 = layer_create(rect);
	layer_set_update_proc(text2, ontext2);
	layer_add_child(body, text2);

	text3 = layer_create(rect);
	layer_set_update_proc(text3, ontext3);
	layer_add_child(body, text3);

	rect.origin.y = MARGIN + FONT0H + MARGIN;
	rect.size.h = FONT1H;
	rect.size.w = SHADOWW;

	hour0 = layer_create(rect);
	layer_set_update_proc(hour0, onhour0);
	layer_add_child(body, hour0);

	rect.origin.x += SHADOWW + SPACING;
	rect.size.w = FONT12W;

	hour1 = layer_create(rect);
	layer_set_update_proc(hour1, onhour1);
	layer_add_child(body, hour1);

	rect.origin.x += FONT12W + SPACING;

	hour2 = layer_create(rect);
	layer_set_update_proc(hour2, onhour2);
	layer_add_child(body, hour2);

	rect.origin.x = MARGIN;
	rect.origin.y = MARGIN + FONT0H + MARGIN + FONT1H + SPACING;
	rect.size.h = FONT2H;
	rect.size.w = SHADOWW;

	minute0 = layer_create(rect);
	layer_set_update_proc(minute0, onminute0);
	layer_add_child(body, minute0);

	rect.origin.x += SHADOWW + SPACING;
	rect.size.w = FONT12W;

	minute1 = layer_create(rect);
	layer_set_update_proc(minute1, onminute1);
	layer_add_child(body, minute1);

	rect.origin.x += FONT12W + SPACING;

	minute2 = layer_create(rect);
	layer_set_update_proc(minute2, onminute2);
	layer_add_child(body, minute2);
}

void
onunload(Window *win)
{
	layer_destroy(body);
	layer_destroy(text0);
	layer_destroy(text1);
	layer_destroy(text2);
	layer_destroy(text3);
	layer_destroy(hour0);
	layer_destroy(hour1);
	layer_destroy(hour2);
	layer_destroy(minute0);
	layer_destroy(minute1);
	layer_destroy(minute2);
}

void
ontick(struct tm *_time, TimeUnits change)
{
	layer_mark_dirty(text0);
	layer_mark_dirty(text1);
	layer_mark_dirty(text2);
	layer_mark_dirty(text3);

	if (change & HOUR_UNIT) {
		layer_mark_dirty(hour0);
		layer_mark_dirty(hour1);
		layer_mark_dirty(hour2);
		vibe(conf.hour);
	}

	if (change & MINUTE_UNIT) {
		layer_mark_dirty(minute0);
		layer_mark_dirty(minute1);
		layer_mark_dirty(minute2);
	}
}

void
onconf(DictionaryIterator *di, void *_ctx)
{
	Tuple *tuple;

	if ((tuple = dict_find(di, MESSAGE_KEY_BG)))
		conf.bg = GColorFromHEX(tuple->value->int32);

	if ((tuple = dict_find(di, MESSAGE_KEY_FG)))
		conf.fg = GColorFromHEX(tuple->value->int32);

	if ((tuple = dict_find(di, MESSAGE_KEY_TEXT0)))
		strncpy(conf.text0, tuple->value->cstring,
			sizeof conf.text0 -1);

	if ((tuple = dict_find(di, MESSAGE_KEY_TEXT1)))
		strncpy(conf.text1, tuple->value->cstring,
			sizeof conf.text1 -1);

	if ((tuple = dict_find(di, MESSAGE_KEY_TEXT2)))
		strncpy(conf.text2, tuple->value->cstring,
			sizeof conf.text2 -1);

	if ((tuple = dict_find(di, MESSAGE_KEY_TEXT3)))
		strncpy(conf.text3, tuple->value->cstring,
			sizeof conf.text3 -1);

	if ((tuple = dict_find(di, MESSAGE_KEY_BTON)))
		conf.bton = atoi(tuple->value->cstring);

	if ((tuple = dict_find(di, MESSAGE_KEY_BTOFF)))
		conf.btoff = atoi(tuple->value->cstring);

	if ((tuple = dict_find(di, MESSAGE_KEY_HOUR)))
		conf.hour = atoi(tuple->value->cstring);

	if ((tuple = dict_find(di, MESSAGE_KEY_SECONDS)))
		conf.seconds = atoi(tuple->value->cstring);

	persist_write_data(CONFKEY, &conf, sizeof conf);
	configure();
}

void
onconnection(bool status)
{
	connected = status;
	vibe(status ? conf.bton : conf.btoff);
	layer_mark_dirty(text0);
	layer_mark_dirty(text1);
	layer_mark_dirty(text2);
	layer_mark_dirty(text3);
}

void
onbattery(BatteryChargeState state)
{
	battery = state.charge_percent;
	charging = state.is_charging;
	layer_mark_dirty(text0);
	layer_mark_dirty(text1);
	layer_mark_dirty(text2);
	layer_mark_dirty(text3);
}

#ifdef PBL_HEALTH
void
onhealth(HealthEventType event, void *_ctx)
{
	switch (event)
	{
	case HealthEventSignificantUpdate:
	case HealthEventMovementUpdate:
		steps = health_service_sum_today(HealthMetricStepCount);
		break;
	case HealthEventHeartRateUpdate:
	case HealthEventSleepUpdate:
	case HealthEventMetricAlert:
		break;
	}
}
#endif

void
ontap(AccelAxisType _axis, i32 _direction)
{
	static u8 count;

	blobi++;
	if (blobi == BLOBN)
		blobi = 0;

	count = 0;

	layer_mark_dirty(text0);
	layer_mark_dirty(text1);
	layer_mark_dirty(text2);
	layer_mark_dirty(text3);

	if (conf.seconds <= 0)
		return;

	app_timer_register(1000, ontimer, &count);
}

void
ontimer(void *ctx)
{
	u8 *count;

	count = ctx;
	(*count)++;

	layer_mark_dirty(text0);
	layer_mark_dirty(text1);
	layer_mark_dirty(text2);
	layer_mark_dirty(text3);

	if (*count >= conf.seconds)
		return;

	app_timer_register(1000, ontimer, count);
}

void
onbody(Layer *layer, GContext *ctx)
{
	graphics_context_set_fill_color(ctx, conf.bg);
	graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
}

void
ontext(Layer *layer, GContext *ctx, char *fmt, Align align)
{
	char str[32];
	Tm *tm;
	GRect rect, glyph;
	GBitmap *fb;
	GBitmapDataRowInfo info;
	i16 x, y, maxx, maxy;
	i16 px, py, maxpx, maxpy;
	u16 i, len;
	u8 *pixels;
	GColor color;
	bool flipx;

	flipx = 0;
	tm = now();

	strftime(str, sizeof str, formatstr(fmt), tm);

	len = strlen(str);
	if (len == 0)
		return;

	rect = layer_get_frame(layer);

	switch (align) {
	case ALIGN_LEFT:
		break;
	case ALIGN_RIGHT:
		rect.origin.x += rect.size.w;
		for (i=0; i<len; i++) {
			glyph = glyphrect(str[i]);
			rect.origin.x -= glyph.size.w;
		}
		rect.origin.x++;
		break;
	case ALIGN_CENTER:
		/* TODO(irek): Pebble Time Round support */
		break;
	}

	maxy = rect.origin.y + rect.size.h;
	fb = graphics_capture_frame_buffer(ctx);
	pixels = gbitmap_get_data(font0);

	for (i=0; i<len; i++) {
		if (str[i] == ICON_FLIPX) {
			flipx = true;
			continue;
		}

		glyph = glyphrect(str[i]);

		/* Each glyph has one pixel gap at the end.
		 * When flipped we have to move that gap to beginning.
		 * By decrementing X we are using gap from previous glyph.
		 */
		if (flipx && glyph.origin.x)
			glyph.origin.x--;

		py = glyph.origin.y;
		maxpx = glyph.origin.x + glyph.size.w;
		maxpy = glyph.origin.y + glyph.size.h;
		
		for (y=rect.origin.y; y<maxy && py<maxpy; y++, py++) {
			info = gbitmap_get_data_row_info(fb, y);
			px = glyph.origin.x;
			maxx = rect.origin.x + rect.size.w;

			if (info.max_x < maxx)
				maxx = info.max_x;

			x = rect.origin.x;

			if (flipx)
				x += glyph.size.w -1;

			for (; x<maxx && x>=0 && px<maxpx; px++) {
				color = conf.bg;

				/* TODO(irek): Magic numbers alert! */
				if (pixels[(768*py+px)>>3] & (0x80>>px%8))
					color = conf.fg;

				drawpixel(info, x, color);

				x += flipx ? -1 : 1;
			}
		}

		rect.origin.x += glyph.size.w;
		rect.size.w -= glyph.size.w;

		flipx = false;
	}

	graphics_release_frame_buffer(ctx, fb);
}

void
ontext0(Layer *layer, GContext *ctx)
{
	ontext(layer, ctx, conf.text0, ALIGN_LEFT);
}

void
ontext1(Layer *layer, GContext *ctx)
{
	ontext(layer, ctx, conf.text1, ALIGN_RIGHT);
}

void
ontext2(Layer *layer, GContext *ctx)
{
	ontext(layer, ctx, conf.text2, ALIGN_LEFT);
}

void
ontext3(Layer *layer, GContext *ctx)
{
	ontext(layer, ctx, conf.text3, ALIGN_RIGHT);
}

void
onhour0(Layer *layer, GContext *ctx)
{
	Tm *tm;
	char buf[4];
	u8 i;
	GDrawCommandList *cmds;
	GDrawCommand *cmd;

	tm = now();
	strftime(buf, sizeof buf, clock_is_24h_style() ? "%H" : "%I", tm);
	i = buf[0] - '0';

	cmds = gdraw_command_image_get_command_list(font1);
	cmd = gdraw_command_list_get_command(cmds, i);

	gdraw_command_set_hidden(cmd, false);
	gdraw_command_set_stroke_color(cmd, conf.fg);
	gdraw_command_set_fill_color(cmd, conf.fg);
	gdraw_command_draw(ctx, cmd);

	dither(layer, ctx, 128);
}

void
onhour1(Layer *layer, GContext *ctx)
{
	Tm *tm;
	char buf[4];
	u8 i;
	GDrawCommandList *cmds;
	GDrawCommand *cmd;

	tm = now();
	strftime(buf, sizeof buf, clock_is_24h_style() ? "%H" : "%I", tm);
	i = buf[0] - '0';

	cmds = gdraw_command_image_get_command_list(font1);
	cmd = gdraw_command_list_get_command(cmds, i);

	gdraw_command_set_hidden(cmd, false);
	gdraw_command_set_stroke_color(cmd, conf.fg);
	gdraw_command_set_fill_color(cmd, conf.fg);
	gdraw_command_draw(ctx, cmd);
}

void
onhour2(Layer *layer, GContext *ctx)
{
	Tm *tm;
	char buf[4];
	u8 i;
	GDrawCommandList *cmds;
	GDrawCommand *cmd;

	tm = now();
	strftime(buf, sizeof buf, clock_is_24h_style() ? "%H" : "%I", tm);
	i = buf[1] - '0';

	cmds = gdraw_command_image_get_command_list(font1);
	cmd = gdraw_command_list_get_command(cmds, i);

	gdraw_command_set_hidden(cmd, false);
	gdraw_command_set_stroke_color(cmd, conf.fg);
	gdraw_command_set_fill_color(cmd, conf.fg);
	gdraw_command_draw(ctx, cmd);
}

void
onminute0(Layer *layer, GContext *ctx)
{
	Tm *tm;
	char buf[4];
	u8 i;
	GDrawCommandList *cmds;
	GDrawCommand *cmd;

	tm = now();
	strftime(buf, sizeof buf, "%M", tm);
	i = buf[0] - '0';

	cmds = gdraw_command_image_get_command_list(font2);
	cmd = gdraw_command_list_get_command(cmds, i);

	gdraw_command_set_hidden(cmd, false);
	gdraw_command_set_stroke_color(cmd, conf.fg);
	gdraw_command_set_fill_color(cmd, conf.fg);
	gdraw_command_draw(ctx, cmd);

	dither(layer, ctx, 128);
}

void
onminute1(Layer *layer, GContext *ctx)
{
	Tm *tm;
	char buf[4];
	u8 i;
	GDrawCommandList *cmds;
	GDrawCommand *cmd;

	tm = now();
	strftime(buf, sizeof buf, "%M", tm);
	i = buf[0] - '0';

	cmds = gdraw_command_image_get_command_list(font2);
	cmd = gdraw_command_list_get_command(cmds, i);

	gdraw_command_set_hidden(cmd, false);
	gdraw_command_set_stroke_color(cmd, conf.fg);
	gdraw_command_set_fill_color(cmd, conf.fg);
	gdraw_command_draw(ctx, cmd);
}

void
onminute2(Layer *layer, GContext *ctx)
{
	Tm *tm;
	char buf[4];
	u8 i;
	GDrawCommandList *cmds;
	GDrawCommand *cmd;

	tm = now();
	strftime(buf, sizeof buf, "%M", tm);
	i = buf[1] - '0';

	cmds = gdraw_command_image_get_command_list(font2);
	cmd = gdraw_command_list_get_command(cmds, i);

	gdraw_command_set_hidden(cmd, false);
	gdraw_command_set_stroke_color(cmd, conf.fg);
	gdraw_command_set_fill_color(cmd, conf.fg);
	gdraw_command_draw(ctx, cmd);
}

int
main(void)
{
	Window *win;
	WindowHandlers wh;
	time_t now;

	/* Window */
	win = window_create();
	wh.load = onload;
	wh.appear = 0;
	wh.disappear = 0;
	wh.unload = onunload;
	window_set_window_handlers(win, wh);
	window_stack_push(win, true);

	/* Time */
	now = time(0);
	ontick(localtime(&now), DAY_UNIT);
	/*
	 * Tick timer is owerwritten in configure() but this is a
	 * default just in case there is something wrong with config
	 * which might happen when phone is disconnected, probably.
	 */
	tick_timer_service_subscribe(MINUTE_UNIT, ontick);

	/* Config */
	conf.bg = GColorBlack;
	conf.fg = GColorWhite;
	strncpy(conf.text0, "#b%%",  sizeof conf.text0);	/* Battery */
	strncpy(conf.text1, "*c*s",  sizeof conf.text1);	/* Icons */
	strncpy(conf.text2, "#s*h",  sizeof conf.text2);	/* health */
	strncpy(conf.text3, "%A %d", sizeof conf.text3);	/* Date */
	conf.bton = VIBE_SILENT;
	conf.btoff = VIBE_SILENT;
	conf.hour = VIBE_SILENT;
	conf.seconds = 0;
	/* TODO(irek): Add some padding in case configuration change
	 * in future version. */
	persist_read_data(CONFKEY, &conf, sizeof conf);
	battery = 0;
	charging = false;
	steps = 0;
	configure();
	app_message_register_inbox_received(onconf);
	app_message_open(app_message_inbox_size_maximum(), 0);

	/* Resources */
	font0 = gbitmap_create_with_resource(RESOURCE_ID_FONT0);
	font1 = gdraw_command_image_create_with_resource(RESOURCE_ID_FONT1);
	font2 = gdraw_command_image_create_with_resource(RESOURCE_ID_FONT2);
	blobi = 0;

	/* Services */
	accel_tap_service_subscribe(ontap);
	connected = connection_service_peek_pebble_app_connection();
	connection_service_subscribe((ConnectionHandlers){ onconnection, 0 });
	battery_state_service_subscribe(onbattery);
	onbattery(battery_state_service_peek());

	/* Main */
	app_event_loop();

	/* Cleanup */
	window_destroy(win);
	gbitmap_destroy(font0);
	gdraw_command_image_destroy(font1);

	return 0;
}
