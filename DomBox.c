#include <stdlib.h>
#include <string.h>
#include <stdio.h> /* debug */

#include <gem.h>

#include "global.h"


static void vTab_delete (DOMBOX *);
static LONG vTab_MinWidth (DOMBOX *);
static LONG vTab_MaxWidth (DOMBOX *);
static void vTab_draw   (DOMBOX *, long x, long y, const GRECT * clip, void *);
static void vTab_format (DOMBOX *, long width, BLOCKER);
struct s_dombox_vtab DomBox_vTab = {
	vTab_delete,
	vTab_MinWidth,
	vTab_MaxWidth,
	vTab_draw,
	vTab_format
};


/*============================================================================*/
DOMBOX *
dombox_ctor (DOMBOX * This, DOMBOX * parent, BOXCLASS class)
{
	memset (This, 0, sizeof (struct s_dombox));
	
	This->_vtab = &DomBox_vTab;
	
	if (This == parent) {
		puts ("dombox_ctor(): This and parent are equal!");
		return This;
	}
	
	if (parent) {
		if (!class) {
			puts ("dombox_ctor(): no class given.");
		}
		if (parent->ChildEnd) parent->ChildEnd->Sibling = This;
		else                  parent->ChildBeg          = This;
		parent->ChildEnd = This;
		This->Parent     = parent;
	} else {
		if (class) {
			puts ("dombox_ctor(): main got class.");
		}
	}
	This->BoxClass = class;
	This->Backgnd  = -1;
	This->Floating = ALN_NO_FLT;
	
	return This;
}

/*============================================================================*/
DOMBOX *
dombox_dtor (DOMBOX * This)
{
	if ((int)This->BoxClass < 0) {
		puts ("dombox_dtor(): already destroyed!");
	}
	if (This->ChildBeg) {
		puts ("dombox_dtor(): has still children!");
	}
	if (This->Parent) {
		DOMBOX * box = This->Parent->ChildBeg;
		if (This == box) {
			if ((This->Parent->ChildBeg = This->Sibling) == NULL) {
				This->Parent->ChildEnd = NULL;
			}
		} else do {
			if (box->Sibling == This) {
				if ((box->Sibling = This->Sibling) == NULL) {
					This->Parent->ChildEnd = box;
				}
				break;
			}
		} while ((box = box->Sibling) != NULL);
		
		This->Parent = NULL;
	}
	This->BoxClass = -1;
	
	return This;
}


/*============================================================================*/
void
dombox_draw (DOMBOX * This, long x, long y, const GRECT * clip, void * hl)
{
	long x1 = x + This->Margin.Lft;
	long x2 = x - This->Margin.Rgt + This->Rect.W -1;
	long y1 = y + This->Margin.Top;
	long y2 = y - This->Margin.Bot + This->Rect.H -1;
	
	if (This->BorderWidth) {
		if (This->BorderColor >= 0) {
			short n = This->BorderWidth;
			PXY b[5];
			b[3].p_x = b[0].p_x = x1;
			b[1].p_x = b[2].p_x = x2;
			b[1].p_y = b[0].p_y = y1;
			b[3].p_y = b[2].p_y = y2;
			vsl_color (vdi_handle, This->BorderColor);
			while(1) {
				b[4] = b[0];
				v_pline (vdi_handle, 5, (short*)b);
				if (!--n) break;
				b[3].p_x = ++b[0].p_x;  b[1].p_x = --b[2].p_x;
				b[1].p_y = ++b[0].p_y;  b[3].p_y = --b[2].p_y;
			}
		} else { /* 3D */
			GRECT b;
			b.g_x = x1;
			b.g_y = y1;
			b.g_w = x2 - x1 +1;
			b.g_h = y2 - y1 +1;
			if (This->BorderColor == -1) { /* outset */
				draw_border (&b, G_WHITE, G_LBLACK, This->BorderWidth);
			} else { /* inset */
				draw_border (&b, G_LBLACK, G_WHITE, This->BorderWidth);
			}
		}
		x1 += This->BorderWidth;
		x2 -= This->BorderWidth;
		y1 += This->BorderWidth;
		y2 -= This->BorderWidth;
	}
	if (This->Backgnd >= 0) {
		PXY p[2];
		p[0].p_x = ((long)clip->g_x >= x1 ? clip->g_x : x1);
		p[0].p_y = ((long)clip->g_y >= y1 ? clip->g_y : y1);
		p[1].p_x = (x2 <= 0x7FFFL ? x2 : 0x7FFF);
		p[1].p_y = (y2 <= 0x7FFFL ? y2 : 0x7FFF);
		if (p[0].p_x > p[1].p_x || p[0].p_y > p[1].p_y) {
			return;
		}
		vsf_color (vdi_handle, This->Backgnd);
		v_bar     (vdi_handle, (short*)p);
	}
	
	(*This->_vtab->draw)(This, x, y, clip, hl);
}

/*----------------------------------------------------------------------------*/
static void
vTab_delete (DOMBOX * This)
{
	while (This->ChildBeg) {
		Delete (This->ChildBeg);
	}
	free (dombox_dtor (This));
}

/*----------------------------------------------------------------------------*/
static LONG
vTab_MinWidth (DOMBOX * This)
{
	DOMBOX * box = This->ChildBeg;
	
	This->MinWidth = 0;
	while (box) {
		long width = dombox_MinWidth (box);
		if (This->MinWidth < width) {
			 This->MinWidth = width;
		}
		box = box->Sibling;
	}
	This->MinWidth += dombox_LftDist (This) + dombox_RgtDist (This);
	
	return This->MinWidth;
}

/*----------------------------------------------------------------------------*/
static LONG
vTab_MaxWidth (DOMBOX * This)
{
	DOMBOX * box = This->ChildBeg;
	
	This->MaxWidth = 0;
	while (box) {
		long width = dombox_MaxWidth (box);
		if (This->MaxWidth < width) {
			 This->MaxWidth = width;
		}
		box = box->Sibling;
	}
	This->MaxWidth += dombox_LftDist (This) + dombox_RgtDist (This);
	
	return This->MaxWidth;
}


/*==============================================================================
 * Returns the box that contains the coordinate px/py which must be relative to
 * the start box's origin.  The rectangle r is set to the extent of this box,
 * relative to px/py (resulting in negative X/Y).  The rectangle excludes all
 * possible overlapping areas of other boxes.  Then px/py will be set to the
 * origin of the box, also relative to the coordinate.
*/
DOMBOX * 
dombox_byCoord (DOMBOX * box, LRECT * r, long * px, long * py)
{
	DOMBOX * cld = box->ChildBeg;
	long     x     = *px;
	long     y     = *py;
	long     c_lft = -x;
	long     c_rgt = -x + box->Rect.W -1;
	long     c_top = -y;
	long     c_bot = -y + box->Rect.H -1;

	r->X = -x;
	r->Y = -y;
	
	while (cld) {
		if (y < cld->Rect.Y) {
			c_bot = r->Y + cld->Rect.Y;
			
		} else if (x < dombox_LftDist (box)) {
			c_rgt = r->X + dombox_LftDist (box);
			
		} else if (x >= box->Rect.W - dombox_RgtDist (box)) {
			c_lft = r->X + box->Rect.W - dombox_RgtDist (box);
		
		} else if (y >= box->Rect.H - dombox_BotDist (box)) {
			c_top = r->Y + box->Rect.H - dombox_BotDist (box);
		
		} else {   /* search through children boxes */
			LONG top = 0, bot;
			while (y >= (bot = cld->Rect.Y + cld->Rect.H)) {
				if (top < bot) {
					top = bot;
				}
				if ((cld = cld->Sibling) == NULL) { /* below the lowest box */
					break;
				}
			}
			c_top = (bot = top) + r->Y;
			while (cld) {
				if (y < cld->Rect.Y) { /* between two boxes */
					c_top = r->Y + bot;
					c_bot = r->Y + cld->Rect.Y;
					cld = NULL;
					break;
				}
				if (x < cld->Rect.X) {
					if (y < cld->Rect.Y + cld->Rect.H) {
						long rgt = r->X + cld->Rect.X;
						if (rgt < c_rgt) {
							c_rgt = rgt;
						}
					}
				} else if (x >= cld->Rect.X + cld->Rect.W) {
					if (y < cld->Rect.Y + cld->Rect.H) {
						long lft = r->X + cld->Rect.X + cld->Rect.W -1;
						if (lft > c_lft) {
							c_lft = lft;
						}
					}
				} else {
					if (y < (bot = cld->Rect.Y + cld->Rect.H)) {
						break;
					} else if (c_top < bot + r->Y) {
						c_top = bot + r->Y;
					}
				}
				cld = cld->Sibling;
			}
			if (cld) {
				r->X += cld->Rect.X;
				r->Y += cld->Rect.Y;
				box  =  cld;
				cld  =  box->ChildBeg;
				x    -= box->Rect.X;
				y    -= box->Rect.Y;
				continue;
			}
		}
		break;   /* finished */
	}
	*px = r->X;
	*py = r->Y;
	if (c_rgt < r->X + box->Rect.W -1) {
		r->W = c_rgt - r->X;
	} else {
		r->W = box->Rect.W;
	}
	if (c_lft > r->X) {
		r->W -= (c_lft - r->X);
		r->X =  c_lft;
	}
	if (c_bot < r->Y + box->Rect.H -1) {
		r->H = c_bot - r->Y;
	} else {
		r->H = box->Rect.H;
	}
	if (c_top > r->Y) {
		r->H -= (c_top - r->Y);
		r->Y =  c_top;
	}
	
	return box;
}


/*----------------------------------------------------------------------------*/
static void
vTab_draw (DOMBOX * This, long x, long y, const GRECT * clip, void * highlight)
{
	DOMBOX * box    = This->ChildBeg;
	long     clip_y = (long)clip->g_y - y;
	
	while (box && box->Rect.Y + box->Rect.H <= clip_y) {
		box = box->Sibling;
	}
	clip_y = clip->g_y + clip->g_h -1;

	while (box) {
		long b_x = x + box->Rect.X;
		long b_y = y + box->Rect.Y;

		if (b_y > clip_y) break;
		
		dombox_draw (box, b_x, b_y, clip, highlight);
		
		box = box->Sibling;
	}
}


/*----------------------------------------------------------------------------*/
static void
vTab_format (DOMBOX * This, long width, BLOCKER blocker)
{
	DOMBOX * box    = This->ChildBeg;
	long     height = dombox_TopDist (This);
	
	This->Rect.W = width;
	width -= dombox_LftDist (This) + dombox_RgtDist (This);
	
	while (box) {
		long blk_width = width - blocker->L.width - blocker->R.width;
		box->Rect.X = dombox_LftDist (This);
		box->Rect.Y = height;

		if (box->MinWidth > blk_width) {
			if (height < blocker->L.bottom) height = blocker->L.bottom;
			if (height < blocker->R.bottom) height = blocker->R.bottom;
			box->Rect.Y = height;
			blocker->L.bottom = blocker->L.width =
			blocker->R.bottom = blocker->R.width = 0;
			blk_width = width;
		}
		
		box->_vtab->format (box, width, blocker);
		
		switch (box->Floating) {
			case ALN_LEFT: {
				long new_bottom = height + box->Rect.H;
				if (blocker->L.bottom < new_bottom)
					 blocker->L.bottom = new_bottom;
				blocker->L.width += box->Rect.W;
			}	break;
			case ALN_RIGHT: {
				long new_bottom = height + box->Rect.H;
				if (blocker->R.bottom < new_bottom)
					 blocker->R.bottom = new_bottom;
				box->Rect.X += blk_width - box->Rect.W; 
				blocker->R.width += box->Rect.W;
			}	break;
			case ALN_CENTER:
				box->Rect.X += (blk_width - box->Rect.W) /2;
			case ALN_NO_FLT:
				height += box->Rect.H;
				if (blocker->L.bottom && blocker->L.bottom < height) {
					blocker->L.bottom = blocker->L.width = 0;
				}
				if (blocker->R.bottom && blocker->R.bottom < height) {
					blocker->R.bottom = blocker->R.width = 0;
				}
		}
		
		box = box->Sibling;
	}

	height += dombox_BotDist (This);
	
	if (height < blocker->L.bottom) {
		 height = blocker->L.bottom;
	}
	if (height < blocker->R.bottom) {
		 height = blocker->R.bottom;
	}
	This->Rect.H = height;
}

/*============================================================================*/
void
dombox_format (DOMBOX * This, long width)
{
	struct blocking_area blocker = { {0, 0}, {0, 0} };
	This->_vtab->format (This, width, &blocker);
}
