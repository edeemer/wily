/*******************************************
 *	drag out a selection, scrolling as you go
 *******************************************/

#include "wily.h"
#include "view.h"
#include "text.h"

static void	frselectf2(Frame *f, Point p0, Point p1, Fcode c);
static void	dclick(View *, ulong, ulong *, ulong *);
static void	erasebar(View *, ulong, ulong);

typedef 	void		(*SelFn)(Frame*, Point, Point, Fcode);

#define BASE(v) ((v)->visible.p0)

/* Return a-b, or 0 */
static ulong
usub (ulong a, ulong b)
{
	return a>b? a-b : 0;
}

/* PRE: 'v' shows the region [p0,p1] selected with 'fn'
 * POST: 'v' shows the region [p0, q] selected with 'fn'
 */
static void
update(View *v, SelFn fn, ulong p0, ulong p1, ulong q)
{
	Frame	*f = 		&v->f;
	Point	pt0, pt1, qt;

	pt0 =	frptofchar(f, usub(p0, BASE(v)));
	pt1 =	frptofchar(f, usub(p1, BASE(v)));
	qt  =		frptofchar(f, usub(q, BASE(v)));

	if(p0 == p1)
		(*fn)(f, pt0, pt1, F&~D);
	if(p1 < q)
		(*fn)(f, pt1, qt, F&~D);
	else
		(*fn)(f, qt, pt1, F&~D);
	if(p0 == q)
		(*fn)(f, pt0, qt, F&~D);

}

/* Toggle the region p0,p1 in 'v' with 'fn' */
static void
toggle(View *v, SelFn fn, ulong p0, ulong p1)
{
	Frame	*f = 		&v->f;
	Point	pt0,pt1;

	pt0 =	frptofchar(f, usub(MIN(p0,p1), BASE(v)));
	pt1 =	frptofchar(f, usub(MAX(p0,p1), BASE(v)));

	(*fn)(f, pt0, pt1, F&~D);
}

/* PRE: The region [p0,p1] in 'v' is indicated with 'fn'
 * POST: We've scrolled towards 'pt', but the region is still indicated.
 */
static void
move(View *v, SelFn fn, Point pt, ulong p0, ulong p1) {
	Rectangle	r = v->f.r;

	assert(!ptinrect(pt, r));

	/* If we moved off the left or right edge of a view with a
	 * scrollbar, or if we're trying to scroll past the top or bottom
	 * of the text, don't do anything.  This avoids a flicker.  It also
	 * avoids scrolling down when the bottom of the text is already
	 * visible.
	 *
	 * The "right" way to fix the flickering
	 * may be to fix the selection so we don't
	 * have to turn it off, then back on.  This would avoid a
	 * flicker while scrolling as well.  I'm not entirely sure
	 * why we need toggle().
	 *
	 * If the flickering is fixed, we will still need to check that
	 * we aren't scrolling down unnecessarily when the bottom of
	 * the text is visible.  That check can appear in a more
	 * appropriate context, however.
	 */
	if (v->scroll && ((pt.y >= r.min.y && pt.y <= r.max.y) ||
				(pt.y < r.min.y && v->visible.p0 == 0) ||
				(pt.y > r.max.y && v->visible.p1 == view_text(v)->length)))
		return;

	toggle(v,fn,p0,p1);
	v->f.p0 = v->f.p1 = 0;

	if (v->scroll) {
		if (pt.y < r.min.y)
			view_linesdown(v, 2, false);
		else if (pt.y > r.max.y)
			view_linesdown(v, 2, true);
	} else {
		if (pt.x < r.min.x)
			view_hscroll(v, true);
		else if (pt.x > r.max.x)
			view_hscroll(v, false);
	}
	toggle(v,fn,p0,p1);
}

/* PRE:  nothing is selected in 'v', our initial desired selection is [p0,p1]
 * POST:  'v' indicates a selection (using Selfn 'fn'), we return the selected
 * range.
 */
static Range
follow(View *v, ulong oldq, ulong p0, ulong p1, Bool selecting, Mouse *m)
{
	ulong	buttons = m->buttons;
	Frame	*f = &v->f;
	ulong	q;
	Event	e;
	ulong	type, timer=0;
	SelFn 	fn;

	fn = selecting? frselectf : frselectf2;
	toggle(v, fn, p0, p1);
	v->selecting = true;
	/*
	 * For subsequent redraws, draw the selection only on the top
	 * left side. The selection will be erased later.
	 */
	if(selecting)
		v->sel.p0 = v->sel.p1 = 0;

	*m= emouse();
	type = 0;
	while(m->buttons == buttons){
		q = frcharofpt(f, m->xy) +v->visible.p0;

		/* We only autoscroll while affecting the selection, not for b2 or b3 */
		if (selecting) {
			/* We want the timer going iff we're outside the rectangle */
			if (ptinrect(m->xy, f->r)) {
				if (timer){
					estoptimer(timer);
					timer=0;
				}
			} else {
				/* We do a move on timer events, or the first mouse event */
				if(timer == type || timer == 0)
					move(v, fn, m->xy, p0, p1);
				if (timer==0)
					timer = etimer(0, SCROLLTIME);
			}
		}
		if(q != oldq && p1 != q){
			update(v, fn, p0, p1, q);
			oldq = p1 = q;
		}
		/* Erase the selection bar left from the previous redraw */
		if(selecting)
			erasebar(v, p0, p1);
		if((type = eread(Emouse|timer,&e))==Emouse)
			*m= e.mouse;
	}
	v->selecting = false;
	if(timer)
		estoptimer(timer);

	return maybereverserange(p0,p1);
}

/* Drag out a selection, starting with 'm' (mouse down),
 * until the mouse buttons change.  Return the selected range.
 */
Range
vselect(View *v, Mouse *m)
{
	Frame	*f = &v->f;
	Bool		selecting;	/* affects selection */
	ulong	orig, p0, p1;
	Range	sel;

	orig = p0 = p1  = frcharofpt(f, m->xy) + BASE(v);

	selecting = m->buttons&LEFT;

	if (selecting) {
		frselectp(f, F&~D);		/*remove old highlighted bit */
		view_setlastselection(v);
		dclick(v, m->msec, &p0, &p1);
	}

	sel = follow(v, orig, p0, p1, selecting, m);

	if(selecting) {
		v->sel = sel;
		v->anchor = sel.p1;
		f->p0 = pclipr(sel.p0, v->visible) - BASE(v);
		f->p1 = pclipr(sel.p1, v->visible) - BASE(v);
	} else {
		frselectf2(f, frptofchar(f, sel.p0 -BASE(v) ), frptofchar(f, sel.p1 -BASE(v)), F&~D);
	}

	assert(view_invariants(v));

	return sel;
}

/* it is assumed p0<=p1 and both were generated by frptofchar() */
static void
frselectf2(Frame *f, Point p0, Point p1, Fcode c)
{
	int n;
	int ht;

	if(p0.x == f->left)
		p0.x = f->r.min.x;
	if(p1.x == f->left)
		p1.x = f->r.min.x;
	ht = f->font->height;
	n = (p1.y-p0.y)/ht;
	/*
	   p0.y += ht - 1;
	   p1.y = p0.y + 1;
	   */
	p1.y = p0.y + ht;
	/* p0.y = (p0.y + p1.y) >> 1; */
	p0.y = p1.y - 2;
	if(f->b == 0)
		berror("frselect2 b==0");
	if(p0.y == f->r.max.y)
		return;
	if(n == 0){
		if(p0.x == p1.x){
			if(p0.x == f->r.min.x)
				p1.x++;
			else
				p0.x--;
		}
		bitblt(f->b, p0, f->b, Rpt(p0, p1), c);
		/* bitblt(f->b, p0, f->d, Rpt(p0, p1), c); */
	}else{
		if(p0.x >= f->r.max.x)
			p0.x = f->r.max.x-1;
		bitblt(f->b, p0, f->b, Rect(p0.x, p0.y, f->r.max.x, p1.y), c);
		while (--n > 0) {
			p0.y += ht;
			p1.y += ht;
			bitblt(f->b, Pt(f->r.min.x, p0.y),
				f->b, Rect(f->r.min.x, p0.y, f->r.max.x, p1.y), c);
		}
		p0.y += ht;
		p1.y += ht;
		bitblt(f->b, Pt(f->r.min.x, p0.y),
				f->b, Rect(f->r.min.x, p0.y, p1.x, p1.y), c);
	}
}

/* Check for double-click.  If 'click' happened not long after 'lastclick',
 * set '*p0' and '*p1' appropriately.  Update 'lastclick'.
 */
static void
dclick(View *v, ulong click, ulong *p0, ulong *p1)
{
	Range	sel;
	static ulong	lastclick;

	/* check for double click */
	if( (v->sel.p0 == *p0 ) && (click < lastclick + DOUBLECLICK )) {
		sel = text_doubleclick(v->t, *p0);
		*p0 = sel.p0;
		*p1 = sel.p1;
		lastclick = 0;
	} else {
		lastclick = click;
	}
}

/* Remove the selection bar from the top left side of the v->f. */
static void
erasebar(View *v, ulong sel0, ulong sel1) {
	Frame *f = &v->f;
	ulong min;
	ulong c;

	min = sel0 < sel1 ? sel0 : sel1;
	c = (min > BASE(v)) ? Zero : ~Zero;
	frselectf(f, frptofchar(f, 0), frptofchar(f, 0), c);
}
