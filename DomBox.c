#include <string.h>
#include <stdio.h> /* debug */

#include <gem.h>

#include "global.h"


/*============================================================================*/
DOMBOX *
dombox_ctor (DOMBOX * This, DOMBOX * parent, BOXCLASS class)
{
	memset (This, 0, sizeof (struct s_dombox));
	
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
	
	return This;
}

/*============================================================================*/
DOMBOX *
dombox_dtor (DOMBOX * This)
{
	if ((int)This->BoxClass < 0) {
		puts ("dombox_ctor(): already destroyed!");
	}
	if (This->ChildBeg) {
		puts ("dombox_ctor(): has still children!");
	}
	if (This->Parent) {
		DOMBOX ** ptr = &This->Parent->ChildBeg;
		while (*ptr) {
			if (*ptr == This) {
				*ptr = This->Sibling;
				break;
			}
			ptr = &(*ptr)->Sibling;
		}
		This->Parent = NULL;
	}
	This->BoxClass = -1;
	
	return This;
}


/*============================================================================*/
void
dombox_draw (DOMBOX * This, long x, long y, const GRECT * clip, void * hl)
{
	if (This->BorderWidth) {
		if (This->BorderColor >= 0) {
			short n = This->BorderWidth;
			PXY b[5];
			b[1].p_x = b[2].p_x = (b[3].p_x = b[0].p_x = x) + This->Rect.W -1;
			b[3].p_y = b[2].p_y = (b[1].p_y = b[0].p_y = y) + This->Rect.H -1;
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
			b.g_x = x;
			b.g_y = y;
			b.g_w = This->Rect.W;
			b.g_h = This->Rect.H;
			if (This->BorderColor == -1) { /* outset */
				draw_border (&b, G_WHITE, G_LBLACK, This->BorderWidth);
			} else { /* inset */
				draw_border (&b, G_LBLACK, G_WHITE, This->BorderWidth);
			}
		}
	}
	if (This->Backgnd >= 0) {
		PXY p[2];
		long xx = x + This->BorderWidth;
		long yy = y + This->BorderWidth;
		p[0].p_x = ((long)clip->g_x >= xx ? clip->g_x : xx);
		p[0].p_y = ((long)clip->g_y >= yy ? clip->g_y : yy);
		xx = x - This->BorderWidth + This->Rect.W -1;
		yy = y - This->BorderWidth + This->Rect.H -1;
		p[1].p_x = (xx <= 0x7FFFL ? xx : 0x7FFF);
		p[1].p_y = (yy <= 0x7FFFL ? yy : 0x7FFF);
		if (p[0].p_x <= p[1].p_x && p[0].p_y <= p[1].p_y) {
			vsf_color (vdi_handle, This->Backgnd);
			v_bar     (vdi_handle, (short*)p);
		}
	}
	
	(void)hl;
	/* (*This->_vtab->draw)(This, x, y, clip, hl); */
}