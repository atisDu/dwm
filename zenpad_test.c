#include <assert.h>
#include <stdio.h>

/* mirrors togglezen's padding math: px -> pts at the dpi kitty uses (Xft.dpi) */
static int zp(int w, int zenwidth, int dpi)
{
	if (w <= zenwidth) return 0;
	return (w - zenwidth) / 2 * 72 / dpi;
}

/* what kitty then turns those pts back into */
static int px(int pt, int dpi) { return pt * dpi / 72; }

static int near(int got, int want) { return got - want < 8 && want - got < 8; }

int main(void)
{
	int dpi = 154; /* this laptop's Xft.dpi */

	/* the round trip through integer pts lands within a few px of zenwidth */
	assert(zp(2880, 1100, dpi) == 416);
	assert(near(2880 - 2 * px(zp(2880, 1100, dpi), dpi), 1100)); /* column survives */
	assert(near(1440 - 2 * px(zp(1440, 1100, dpi), dpi), 1100)); /* half tile too */
	assert(zp(1100, 1100, dpi) == 0);  /* exact fit  -> no padding */
	assert(zp(700, 1100, dpi) == 0);   /* narrow tile-> never eats the text */
	assert(zp(2880, 1100, 96) == 667); /* non-hidpi screen still sane */

	/* the bug this replaced: pts computed at 96dpi, rendered by kitty at 154 */
	assert(2880 - 2 * px(667, 154) < 30);

	puts("zen padding ok");
	return 0;
}
