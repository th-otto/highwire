/* @(#)highwire/Table.c
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "token.h"
#include <gem.h>

#ifdef __PUREC__
# define PARSER struct s_parser * /* for forward decl */
#endif

#include "global.h"
#include "Loader.h"
#include "parser.h"
#include "fontbase.h"
#include "Table.h"

#define t_Width      Paragraph->Box.Rect.W
#define t_Height     Paragraph->Box.Rect.H
#define t_MinWidth   Paragraph->Box.MinWidth
#define t_MaxWidth   Paragraph->Box.MaxWidth
#define t_SetWidth   Paragraph->Box.SetWidth
#define t_Backgnd    Paragraph->Box.Backgnd
#define t_BorderW    Paragraph->Box.BorderWidth
#define t_BorderC    Paragraph->Box.BorderColor

#define c_OffsetX Box.Rect.X
#define c_OffsetY Box.Rect.Y
#define c_Width   Box.Rect.W
#define c_Height  Box.Rect.H
#define c_Backgnd Box.Backgnd

#ifdef DEBUG
	#define _DEBUG
#else
/*	#define _DEBUG */
#endif


static struct s_dombox_vtab table_vTab = { 0, };
static LONG     vTab_MinWidth (DOMBOX *);
static LONG     vTab_MaxWidth (DOMBOX *);
static DOMBOX * vTab_ChildAt  (DOMBOX *, LRECT *, long x, long y, long clip[4]);
static void     vTab_draw   (DOMBOX *, long x, long y, const GRECT *, void *);
static void     vTab_format (DOMBOX *, long width, BLOCKER);


/*============================================================================*/
void
delete_table (TABLE * _table)
{
	TABLE table = *_table;
	if (table) {
		TAB_ROW row = table->Rows;
		while (row) {
			TAB_ROW  next_row = row->NextRow;
			TAB_CELL cell     = row->Cells;
			while (cell) {
				TAB_CELL next_cell = cell->RightCell;
				Delete (&cell->Box);
				cell = next_cell;
			}
			free (row);
			row = next_row;
		}
		if (table->Minimum)  free (table->Minimum);
		if (table->Maximum)  free (table->Maximum);
		if (table->Percent)  free (table->Percent);
		if (table->ColWidth) free (table->ColWidth);
		free (table);
		*_table = NULL;
	}
}


/*============================================================================*/
void
table_start (PARSER parser, WORD color, H_ALIGN floating, WORD height,
             WORD width, WORD spacing, WORD padding, WORD border, BOOL special)
{
	TEXTBUFF current = &parser->Current;
	PARAGRPH par     = add_paragraph (current, 0);
	TABLE    table   = malloc (sizeof (struct s_table));
	TBLSTACK stack   = malloc (sizeof (struct s_table_stack));
	/*puts("table_start");*/
	stack->Table     = table;
	stack->PrevRow   = NULL;
	stack->WorkRow   = NULL;
	stack->WorkCell  = NULL;
	stack->NumCells  = 0;
	stack->isSpecial = special;
	stack->SavedCurr = *current;
	stack->_debug = 'A';
	
	par->Table          = table;
	par->paragraph_code = PAR_TABLE;
	if (!*(long*)&table_vTab) {
		table_vTab          = DomBox_vTab;
		table_vTab.MinWidth = vTab_MinWidth;
		table_vTab.MaxWidth = vTab_MaxWidth;
		table_vTab.ChildAt  = vTab_ChildAt;
		table_vTab.draw     = vTab_draw;
		table_vTab.format   = vTab_format;
	}
	par->Box._vtab = &table_vTab;
	par->Box.Floating    = floating;
	par->Box.BoxClass    = BC_TABLE;
	par->Box.Padding.Top = par->Box.Padding.Bot =
	par->Box.Padding.Lft = par->Box.Padding.Rgt = spacing;
	
	current->tbl_stack = stack;
	current->lst_stack = NULL;
	current->font      = fontstack_setup (&current->fnt_stack,
	                                      parser->Frame->text_color);

	table->Paragraph = par;
	table->Rows      = NULL;
	table->NumCols   = 0;
	table->FixCols   = 0;
	table->NumRows   = 0;
	table->Minimum   = NULL;
	table->Maximum   = NULL;
	table->Percent   = NULL;
	table->ColWidth  = NULL;

	table->t_Backgnd = (color != current->backgnd ? color : -1);
	table->t_BorderW = border;
	table->t_BorderC = -1; /* 3D outset */
	table->Spacing   = spacing;
	table->Padding   = padding + (border ? 1 : 0);
	table->t_SetWidth = width;
	table->SetHeight = height;
}


/*============================================================================*/
void
table_row (TEXTBUFF current, WORD color, H_ALIGN h_align, V_ALIGN v_align,
           WORD height, BOOL beginNend)
{
	TBLSTACK stack = current->tbl_stack;
	TABLE    table = stack->Table;
	TAB_ROW  row   = NULL;

/*	printf("table_row(%i)\n", table->NumRows);*/

	if (stack->WorkRow) {
		if (stack->NumCells) {
			stack->PrevRow  = stack->WorkRow;
			stack->NumCells = 0;
		
		} else { /* the actual row is empty */
			
			if (stack->PrevRow) {   /* all rowspans reaching in the empty row
			                         * need to be decremented */
				TAB_CELL cell = stack->PrevRow->Cells;
				while (cell) {
					if (cell->RowSpan > 1) {
						cell->RowSpan--;
					} else if (cell->RowSpan < 0) {
						TAB_CELL col = cell->DummyFor;
						if (col) {
							col->RowSpan--;
							while ((col = col->BelowCell) != NULL) {
								col->RowSpan++;
							}
						}
					}
					cell = cell->RightCell;
				}
			}
			
			if (beginNend) {
				/* reuse empty row */
				row = stack->WorkRow;
			
			} else {
				if (stack->PrevRow) {
					stack->PrevRow->NextRow = NULL;
					table->NumRows--;
				} else if (table->Rows == stack->WorkRow) {
					table->Rows    = NULL;
					table->NumRows = 0;;
				}
				free (stack->WorkRow);
			}
		}
		stack->WorkRow = NULL;
	}
	
	if (beginNend) {
		if (!row) {
			row = malloc (sizeof (struct s_table_row));
			row->Cells   = NULL;
			row->NextRow = NULL;
			table->NumRows++;
		}
		row->Color     = (color != table->t_Backgnd ? color : -1);
		row->MinHeight = height;
		row->Height    = table->Padding *2;
	
		stack->AlignH = h_align;
		stack->AlignV = v_align;
	
		if (!table->Rows) {
			table->Rows = row;
	
		} else if (stack->PrevRow){
			stack->PrevRow->NextRow = row;
		}
		stack->WorkRow = row;
	}
}


/*----------------------------------------------------------------------------*/
static TAB_CELL
new_cell (DOMBOX * parent, TAB_CELL left_side, short padding)
{
	TAB_CELL cell = malloc (sizeof (struct s_table_cell));

	if (left_side) {
		left_side->RightCell = cell;
	}
	dombox_ctor (&cell->Box, parent, BC_STRUCT);
	if (padding) {
		cell->Box.Padding.Top = cell->Box.Padding.Bot =
		cell->Box.Padding.Lft = cell->Box.Padding.Rgt = padding;
	}
	cell->Box.BorderWidth = (parent->BorderWidth ? 1 : 0);
	cell->Box.BorderColor = -2; /* 3D inset */
	cell->ColSpan   = 1;
	cell->RowSpan   = 1;
	cell->DummyFor  = NULL;
	cell->RightCell = NULL;
	cell->BelowCell = NULL;

	cell->_debug = '.';

	return cell;
}


/*============================================================================*/
void
table_cell (PARSER parser, WORD color, H_ALIGN h_align, V_ALIGN v_align,
            WORD height, WORD width, UWORD rowspan, UWORD colspan)
{
	TBLSTACK stack = parser->Current.tbl_stack;
	TABLE    table = stack->Table;
	DOMBOX * box   = &table->Paragraph->Box;
	TAB_ROW  row;
	TAB_CELL cell;

/*	printf("table_cell('%c')\n", stack->_debug);*/
	
	if (stack->WorkCell) {
		paragrph_finish (&parser->Current);
		parser->Current.font = fontstack_setup (&parser->Current.fnt_stack,
	                                           parser->Frame->text_color);
	}
	
	if (!stack->WorkRow) {
		table_row (&parser->Current, -1, ALN_LEFT, ALN_MIDDLE, 0, TRUE);
	}
	row = stack->WorkRow;
	
	if (!stack->NumCells) {
		/*
		 * the first cell of the row, so we create a new row of cells with
		 * respect of the previous row's (if any) rowspan information.
		 */
		if (stack->PrevRow) {
			TAB_CELL prev = stack->PrevRow->Cells;
			if (!prev) {
				prev = stack->PrevRow->Cells = new_cell (box, NULL, table->Padding);
			}
			cell = NULL;
			do {
				prev->BelowCell = cell = new_cell (box, cell, table->Padding);
				if (prev->RowSpan > 1) {
					cell->DummyFor = prev;
					cell->RowSpan  = 2 - prev->RowSpan;
					cell->ColSpan  = prev->ColSpan;
				} else if (prev->RowSpan < 0) {
					cell->DummyFor = prev->DummyFor;
					cell->RowSpan  = 1 + prev->RowSpan;
					cell->ColSpan  = prev->ColSpan;
				}
			} while ((prev = prev->RightCell) != NULL);
			row->Cells = stack->PrevRow->Cells->BelowCell;
		}
		cell            = row->Cells;
		stack->NumCells = 1;

	} else {
		cell = stack->WorkCell->RightCell;
		stack->NumCells++;
	}

	/* now search the next cell that isn't a dummy of a row/colspan
	 */
	while (cell && cell->DummyFor) {
		stack->WorkCell = cell;
		cell            = cell->RightCell;
		stack->NumCells++;
	}

	/* if we haven't a cell here we need to create a new one
	 */
	if (!cell) {
		cell = new_cell (box, stack->WorkCell, table->Padding);
		if (!row->Cells) {
			row->Cells = cell;
		}
	}
	stack->WorkCell = cell;

	cell->_debug = stack->_debug++;
	
	parser->Current.parentbox = &cell->Box;
	new_paragraph (&parser->Current);
	
	cell->Box.TextAlign = parser->Current.paragraph->Box.TextAlign = h_align;
	cell->AlignV        = v_align;
	cell->c_Height      = (height <= 1024 ? height : 1024);
	cell->c_Width       = (width  <= 1024 ? width  : 0);
	
	if (color < 0) {
		color = row->Color;
	} else if (color == table->t_Backgnd) {
		color = -1;
	}
	if (color >= 0) {
		parser->Current.backgnd = cell->c_Backgnd = color;
	} else if (table->t_Backgnd >= 0) {
		parser->Current.backgnd = table->t_Backgnd;
	} else {
		parser->Current.backgnd = stack->SavedCurr.backgnd;
	}
	
	if (rowspan > 1) {
		cell->RowSpan = rowspan;
	}
	if (colspan > 1) {
		short span = 2 - colspan;
		cell->ColSpan = colspan;
		do {
			TAB_CELL next = (stack->WorkCell->RightCell
			                 ? stack->WorkCell->RightCell
			                 : new_cell (box, stack->WorkCell, table->Padding));
			next->RowSpan = stack->WorkCell->RowSpan;
			stack->WorkCell = next;
			if (!stack->WorkCell->DummyFor) {
				stack->WorkCell->DummyFor = cell;
				stack->WorkCell->ColSpan  = span;
			}
			stack->NumCells++;
		} while (span++);
	}

	if (!stack->PrevRow) {
		table->NumCols = stack->NumCells;

	} else if (table->NumCols < stack->NumCells) {
		/*
		 * if this row has more cells than the previous rows we have to expand
		 * all other rows
		 */
		TAB_CELL last = table->Rows->Cells;
		while (last->RightCell) last = last->RightCell;
		do {
			TAB_CELL prev = last->BelowCell;
			TAB_CELL next = new_cell (box, last, table->Padding);
			while (!prev->RightCell) {
				next = next->BelowCell = new_cell (box, prev, table->Padding);
				prev = prev->BelowCell;
			}
			next->BelowCell = prev->RightCell;
			last            = last->RightCell;
		} while (++table->NumCols < stack->NumCells);
	}
	
	/* now reset to valid values */
	stack->WorkCell =  cell;
	stack->NumCells -= cell->ColSpan -1;
}


/*----------------------------------------------------------------------------*/
static void
adjust_rowspans (TAB_CELL column, int num_rows)
{
	do if (column->RowSpan > num_rows) {
		TAB_CELL cell = column;
		short    span = 2 - num_rows;
		column->RowSpan = num_rows;
		while ((cell = cell->BelowCell) != NULL) {
			cell->RowSpan = span;
			if (!span++) break;
		}
	} while ((column = column->RightCell) != NULL);
}


/*----------------------------------------------------------------------------*/
static void
spread_width (long * list, short span, short spacing, long width)
{
	short i = span;

	width -= *list;
	while (--i && (width -= list[i] + spacing) > 0);

	if (width > 0) {
		do {
			short w = width / span;
			list[--span] += w;
			width        -= w;
		} while (span > 1);
		*list += width;
	}
}


/*----------------------------------------------------------------------------*/
static void
free_stack (TEXTBUFF current)
{
	TBLSTACK stack = current->tbl_stack;
	
	fontstack_clear (&current->fnt_stack);
	*current = stack->SavedCurr;

	add_paragraph (current, 0);
	
	free (stack);
}

/*============================================================================*/
void
table_finish (PARSER parser)
{
	TBLSTACK stack = parser->Current.tbl_stack;
	TABLE    table = stack->Table;
	TAB_ROW  row;
	TAB_CELL column;
	short    padding;
	long   * minimum, * maximum, * percent, * fixed;
	int      i;
	
/*	printf ("table_finish() %i * %i \n", table->NumCols, table->NumRows);*/
	
	if (stack->WorkCell) {
		paragrph_finish (&parser->Current);
	}
	
	if (stack->WorkRow) {
		table_row (&parser->Current, -1,0,0,0, FALSE);
	}
	
	if (!table->Rows) {
		#ifdef _DEBUG
			printf ("table_finish(): no rows!\n");
		#endif
		free_stack (&parser->Current);
		return;
	} else if (!table->NumCols) {
		#ifdef _DEBUG
			printf ("table_finish(): no columns!\n");
		#endif
		free_stack (&parser->Current);
		return;
	}
	
	table->t_MinWidth = table->t_MaxWidth
	                  = dombox_LftDist (&table->Paragraph->Box)
	                  + dombox_RgtDist (&table->Paragraph->Box);
	
	padding = table->Padding *2;
	table->Minimum = malloc (sizeof (*table->Minimum) * table->NumCols);
	for (i = 0; i < table->NumCols; table->Minimum[i++] = padding);
	table->Maximum = malloc (sizeof (*table->Maximum) * table->NumCols);
	for (i = 0; i < table->NumCols; table->Maximum[i++] = 0);
	table->Percent = malloc (sizeof (*table->Percent) * table->NumCols);
	for (i = 0; i < table->NumCols; table->Percent[i++] = 0);
	table->ColWidth = malloc (sizeof (*table->ColWidth) * table->NumCols);
	for (i = 0; i < table->NumCols; table->ColWidth[i++] = 0);

#if 0
{
	int y = table->NumRows;
	row = table->Rows;
	puts("---------------------");
	do {
		TAB_CELL cell = row->Cells;
		int x = table->NumCols;
		y--;
		do {
			printf ("%c", (cell->DummyFor ? tolower (cell->DummyFor->_debug) : cell->_debug));
			printf ("%+i%+i ", cell->ColSpan, cell->RowSpan);
			if ((!--x && cell->RightCell) || (x && !cell->RightCell)) {
				printf ("BOINK %i -> %p \n", x, cell->RightCell);
				exit(EXIT_FAILURE);
			}
			if ((!y && cell->BelowCell) || (y && !cell->BelowCell)) {
				printf ("PLONK %i -> %p \n", y, cell->BelowCell);
				exit(EXIT_FAILURE);
			}
		} while ((cell = cell->RightCell) != NULL);
		puts("");
	} while ((row = row->NextRow) != NULL);
}
#endif
	
	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	 * Step 1:  walk over all table cells and set the column's minimum/maximum/
	 *          percentage width for cells with single colspan and the row's
	 *          minimum height for cells with single rowspan.
	 */
	row = table->Rows;
	i   = table->NumRows;
	do {
		TAB_CELL cell = row->Cells;
		fixed   = table->ColWidth;
		percent = table->Percent;
		minimum = table->Minimum;
		maximum = table->Maximum;

		adjust_rowspans (cell, i--);
		do {
			if (cell->Box.ChildBeg) {
				if (cell->c_Width < 0 && table->t_SetWidth > 0) {
					short width = table->t_SetWidth - table->t_MinWidth;
					cell->c_Width = (-cell->c_Width * width +512) /1024;
					if (cell->c_Width <= 0) cell->c_Width = 1;
				}
				if (cell->ColSpan == 1) {
					long width = dombox_MinWidth (&cell->Box);
					if (width <= padding) {
						cell->c_Width = 0;
					}
					if (cell->c_Width > 0) {
						if (width < cell->c_Width) width = cell->c_Width;
						if (width < *minimum)    width = *minimum;
						if (width < *fixed)      width = *fixed;
						*minimum = *fixed = width;
					} else {
						if (*minimum < width) *minimum = width;
						width = dombox_MaxWidth (&cell->Box);
						if (*maximum < width) *maximum = width;
						if (cell->c_Width < 0) {
							if (table->NumCols > 1) {
								*percent = -cell->c_Width;
							} else {
								cell->c_Width = 0;
							}
						}
					}
				}
				if (cell->RowSpan == 1) {
					if (row->MinHeight < cell->c_Height) {
						row->MinHeight = cell->c_Height;
					}
				}
			}
			fixed++;
			percent++;
			minimum++;
			maximum++;
		} while ((cell = cell->RightCell) != NULL);
	} while ((row = row->NextRow) != NULL);

	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	 * Step 2:  walk over all table columns and spread the minimum/maximum width
	 *          of cells with multiple colspan.
	 */
	column  = table->Rows->Cells;
	fixed   = table->ColWidth;
	percent = table->Percent;
	minimum = table->Minimum;
	maximum = table->Maximum;
	do {
		TAB_CELL cell = column;
		do {
			if (cell->Box.ChildBeg && cell->ColSpan > 1) {
				long width = dombox_MinWidth (&cell->Box);
				spread_width (minimum, cell->ColSpan, table->Spacing, width);
				if (cell->c_Width > 0) {
					short empty = 0;
					if (width < cell->c_Width) width = cell->c_Width;
					for (i = 0; i < cell->ColSpan; i++) {
						if (!fixed[i]) {
							empty++;
							width -= minimum[i];
						} else {
							width -= fixed[i];
						}
					}
					if (empty) {
						i = -1;
						while(1) {
							if (!fixed[++i]) {
								fixed[i] = minimum[i];
								if (width > 0) {
									short w = width / empty;
									fixed[i] += w;
									width    -= w;
								}
								if (!--empty) break;
							}
						}
					}
				} else {
					spread_width (maximum, cell->ColSpan, table->Spacing,
					              dombox_MaxWidth (&cell->Box));
					if (cell->c_Width < 0 && cell->ColSpan < table->NumCols) {
						short empty = 0;
						percent = table->Percent + (fixed - table->ColWidth);
						width   = -cell->c_Width;
						for (i = 0; i < cell->ColSpan; i++) {
							if (!percent[i]) {
								empty++;
							} else {
								width -= percent[i];
							}
						}
						if (width > 0) {
							i = cell->ColSpan;
							if (!empty) {
								do {
									short w = width / i--;
									percent[i] += w;
									width      -= w;
								} while (i);
							
							} else do {
								if (!percent[--i]) {
									short w = width / empty--;
									percent[i] += w;
									width      -= w;
								}
							} while (empty);
						}
					}
				}
			}
		} while ((cell = cell->BelowCell) != NULL);
		if (*fixed) {
			table->FixCols++;
			if (*fixed < *minimum) *fixed = *minimum;
			*maximum = *minimum = *fixed;
			*percent = 0;
		} else if (*maximum <= *minimum) {
			*maximum = *minimum +1;
		}
		fixed++;
		percent++;
		table->t_MinWidth += *(minimum++) + table->Spacing;
		table->t_MaxWidth += *(maximum++) + table->Spacing;
	} while ((column = column->RightCell) != NULL);
	table->t_MinWidth -= table->Spacing;
	table->t_MaxWidth -= table->Spacing;

	if (table->FixCols == table->NumCols) {
		table->t_SetWidth = table->t_MaxWidth = table->t_MinWidth;

	} else if (table->t_SetWidth > 0) {
		if (table->t_SetWidth < table->t_MinWidth) {
			 table->t_SetWidth = table->t_MinWidth;
		} else {
			short spread = table->t_SetWidth - table->t_MinWidth;
			table->t_MinWidth =  table->t_SetWidth;
			table->t_SetWidth += spread;
		}
		table->t_MaxWidth = table->t_MinWidth;
	}

	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	 * Step 3:  walk over all table rows and spread the minimum height
	 *          of cells with multiple rowspan.
	 */
	row = table->Rows;
	do {
		TAB_CELL cell = row->Cells;
		do {
			if (cell->Box.ChildBeg && cell->RowSpan > 1) {
				short span   = cell->RowSpan -1;
				short height = cell->c_Height - row->MinHeight;
				TAB_ROW r    = row->NextRow;
				while ((height -= r->MinHeight + table->Spacing) > 0 && --span) {
					r = r->NextRow;
				}
				if (height > 0) {
					span = cell->RowSpan;
					r    = row;
					do {
						short h = height / span;
						r->MinHeight += h;
						height    -= h;
						r = r->NextRow;
					} while (--span > 1);
					r->MinHeight += height;
				}
			}
		} while ((cell = cell->RightCell) != NULL);
	} while ((row = row->NextRow) != NULL);

	free_stack (&parser->Current);
}


/*----------------------------------------------------------------------------*/
static void
vTab_format (DOMBOX * This, long max_width, BLOCKER blocker)
{
	TABLE   table = ((PARAGRPH)This)->Table;
	TAB_ROW row;
	long    set_width, y;
	
	(void)blocker;
	
	This->Rect.X += blocker->L.width;
	This->Rect.W -= blocker->L.width + blocker->R.width;
	max_width    -= blocker->L.width + blocker->R.width;
	
	if (!table->NumCols) {
		#ifdef _DEBUG
			printf ("table_calc(): no columns!\n");
		#endif
		return;
	}
	/*printf ("table_calc(%li) %i\n", max_width, table->t_SetWidth);*/


	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	 * Step 1:  adjust the given width to the table's boundarys.
	 */
	if (table->t_SetWidth > 0) {
		set_width = table->t_SetWidth;
	} else if (table->t_SetWidth) {
		set_width = (max_width * -table->t_SetWidth + 512) / 1024;
	} else {
		set_width = max_width;
	}
	if (set_width < table->t_MinWidth) {
		 set_width = table->t_MinWidth;
	}

	if (set_width == table->t_Width) {
	/*	return table->t_Height; */
	}

	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	 * Step 2:  calculate the column widths.
	 */
	if (set_width == table->t_MinWidth) { /* - - - - - - - - - - - - - - - - -
		 *
		 * Step 2.a:  for a minimum table width the stored column minimums
		 *            are used.
		 */
		long * col_width = table->ColWidth;
		long * minimum   = table->Minimum;
		int    i         = table->NumCols;
		do {
			*(col_width++) = *(minimum++);
		} while (--i);

	} else if (table->t_SetWidth) { /* - - - - - - - - - - - - - - - - - - - -
		 *
		 * Step 2.b:  for a defined width policy spread the extra size evenly
		 *            over all columns that hasn't a fixed width.
		 */
		long * col_width = table->ColWidth;
		long * percent   = table->Percent;
		long * maximum   = table->Maximum;
		long * minimum   = table->Minimum;
		short  rest      = table->NumCols - table->FixCols;
		short  i         = table->NumCols;
		short  spread    = set_width - table->t_MinWidth;
		short  resize    = 0;
		do {
			if (*percent) {
				long width = (set_width * *percent +512) /1024 - *minimum;
				*col_width = *minimum;
				if (width > spread) {
					*col_width += spread;
					spread     =  0;
				} else if (width > 0) {
					*col_width += width;
					spread     -= width;
				}
				rest--;
			}
			col_width++;
			percent++;
			maximum++;
			minimum++;
		} while (--i);
		if (rest) {
			col_width = table->ColWidth;
			percent   = table->Percent;
			maximum   = table->Maximum;
			minimum   = table->Minimum;
			i         = rest;
			do {
				if (!*percent && *maximum > *minimum) {
					short w = spread / i--;
					if (w >= *maximum - *minimum) {
						w = *maximum - *minimum -1;
					} else {
						resize++;
					}
					*col_width = w + *minimum;
					spread    -= w;
				}
				col_width++;
				percent++;
				maximum++;
				minimum++;
			} while (i);
		}
		if (spread > 0) {
			col_width = table->ColWidth;
			percent   = table->Percent;
			maximum   = table->Maximum;
			minimum   = table->Minimum;
			i = (resize ? resize : rest ? rest : table->NumCols - table->FixCols);
			do {
				if ((resize
				     ? *(maximum++) > *(minimum++) +1 &&           !*percent
				     : *(maximum++) > *(minimum++)    && (!rest || !*percent))) {
					short w = spread / i--;
					*col_width += w;
					spread     -= w;
				}
				col_width++;
				percent++;
			} while (i);
		}
		
		if (table->t_SetWidth > table->t_MinWidth) {
			col_width = table->ColWidth;
			minimum   = table->Minimum;
			i         = table->NumCols;
			do {
				*(minimum++) = *(col_width++);
			} while (--i);

			set_width = table->t_SetWidth = table->t_MinWidth;
		}

	} else { /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		 *
		 * Step 2.c:  no width was given, so calculate the needed table width
		 *            while spreading the available width over all non-fixed
		 *            columns.
		 */
		long * col_width = table->ColWidth;
		long * percent   = table->Percent;
		long * maximum   = table->Maximum;
		long * minimum   = table->Minimum;
		short  spread    = set_width - table->t_MinWidth;
		short  rest      = table->NumCols;
		int    i         = table->NumCols;
		
		if (max_width > table->t_MaxWidth) {
			 max_width = table->t_MaxWidth;
		}
		max_width -= table->t_MinWidth;
		
		/* first mark all columns to be spread with a negative value */
		do {
			*col_width = *minimum;
			if (*percent) {
				long width = (max_width * *percent +512) /1024;
				if (width > spread) {
					*col_width = spread + *minimum;
					spread = 0;
				} else if (width > 0) {
					*col_width = width + *minimum;
					spread -= width;
				}
				rest--;
			} else if (*maximum > *minimum) {
				*col_width -= *maximum;
			} else {
				rest--;
			}
			col_width++;
			percent++;
			maximum++;
			minimum++;
		} while (--i);
		while (rest) {
			short width = -(spread / rest);
			BOOL  found = FALSE;
			col_width = table->ColWidth;
			maximum   = table->Maximum;
			minimum   = table->Minimum;
			i = table->NumCols;
			if (width) do {
				if (*col_width < 0 && width <= *col_width) {
					spread    += *col_width;
					*col_width = *maximum;
					found      = TRUE;
					if (!--rest) break;
				}
				col_width++;
				maximum++;
				minimum++;
			} while (--i);
			if (!found) {
				col_width = table->ColWidth;
				minimum   = table->Minimum;
				i = table->NumCols;
				do {
					if (*col_width <= 0) {
						width      = spread / rest;
						spread    -= width;
						*col_width = width + *minimum;
						if (!--rest) break;
					}
					col_width++;
					minimum++;
				} while (--i);
			}
		}
		set_width -= spread;
	}

	table->t_Width = set_width;
	
	/* - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - -
	 * now calculate cells hights that spread exactly over one row
	 */
	row = table->Rows;
	do {
		long   * col_width = table->ColWidth;
		TAB_CELL cell      = row->Cells;
		row->Height = row->MinHeight;
		while (cell) {
			long width = *(col_width++);
			if (cell->Box.ChildBeg) {
				short span = cell->ColSpan -1;
				while (span--) width += col_width[span] + table->Spacing;
				dombox_format (&cell->Box, width);
				if (cell->RowSpan == 1 && row->Height < cell->c_Height) {
					row->Height = cell->c_Height;
				}
			} else {
				cell->c_Width = width;
			}
			cell = cell->RightCell;
		}
	} while ((row = row->NextRow) != NULL);
	
	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	* now all cells are calculated that spread more than one row.  we also 
	* get now the needed table hight.
	*/
	table->t_Height = dombox_TopDist (&table->Paragraph->Box);
	row = table->Rows;
	do {
		TAB_CELL cell = row->Cells;
		while (cell) {
			if (cell->Box.ChildBeg && cell->RowSpan > 1) {
				short span   = cell->RowSpan -1;
				short height = cell->c_Height - row->Height;
				TAB_ROW r    = row->NextRow;
				while ((height -= r->Height + table->Spacing) > 0 && --span) {
					r = r->NextRow;
				}
				if (height > 0) {
					span = cell->RowSpan;
					r    = row;
					do {
						short h = height / span;
						r->Height += h;
						height    -= h;
						r = r->NextRow;
					} while (--span > 1);
					r->Height += height;
				}
			}
			cell = cell->RightCell;
		}
		table->t_Height += row->Height + table->Spacing;
	} while ((row = row->NextRow) != NULL);
	table->t_Height += dombox_BotDist (&table->Paragraph->Box) - table->Spacing;
	
	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	 * in case of a given table hight we need to spread the calculated row
	 * hights.
	 */
	if (table->SetHeight > table->t_Height) {
		short height = table->SetHeight - table->t_Height;
		short num    = table->NumRows;
		row = table->Rows;
		do {
			short h = height / num--;
			row->Height += h;
			height      -= h;
		} while ((row = row->NextRow) != NULL);
		table->t_Height = table->SetHeight;
	}
	
	/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	 * Last Step:  calculate the cells's offsets and adjust their contents
	 *             for the case of valign=middle/bottom.
	*/
	row = table->Rows;
	y   = dombox_TopDist (&table->Paragraph->Box);
	do {
		long   * col_width = table->ColWidth;
		TAB_CELL cell      = row->Cells;
		long     x         = table->t_BorderW + table->Spacing;
		do {
			long height = row->Height;
			if (cell->RowSpan > 1) {
				short   i = cell->RowSpan -1;
				TAB_ROW r = row;
				do {
					r = r->NextRow;
					height += r->Height + table->Spacing;
				} while (--i);
			}
			if (height != cell->c_Height) {
				dombox_stretch (&cell->Box, height, cell->AlignV);
			}
			cell->c_OffsetX = x;
			cell->c_OffsetY = y;
			x += *(col_width++) + table->Spacing;
		} while ((cell = cell->RightCell) != NULL);
		y += row->Height + table->Spacing;
	} while ((row = row->NextRow) != NULL);
}


/*----------------------------------------------------------------------------*/
static LONG
vTab_MinWidth (DOMBOX * This)
{
	return This->MinWidth;
}

/*----------------------------------------------------------------------------*/
static LONG
vTab_MaxWidth (DOMBOX * This)
{
	return This->MaxWidth;
}


/*----------------------------------------------------------------------------*/
static DOMBOX *
vTab_ChildAt (DOMBOX * This, LRECT * r, long x, long y, long clip[4])
{
	TABLE    table = ((PARAGRPH)This)->Table;
	TAB_ROW  row   = (table->NumCols ? table->Rows : NULL);
	TAB_CELL cell  = NULL;
	
	clip[0] = r->X;
	clip[1] = r->Y;
	
	if (row) {
		long height = dombox_TopDist (This);
		if (y < height) {
			clip[3] = r->Y + height;
			return NULL;
		}
		while (y >= (height += row->Height + table->Spacing)
		       && (row = row->NextRow) != NULL) ;
		if (!row) {
			clip[1] = r->Y + height -1 - table->Spacing;
			return NULL;
		}
		cell = row->Cells;
	}
	if (cell) {
		long * col_w = table->ColWidth;
		long   width = dombox_LftDist (This);
		if (x < width) {
			clip[2] = r->X + width;
			return NULL;
		}
		while (x >= (width += *(col_w++) + table->Spacing)
		       && (cell = cell->RightCell) != NULL) ;
		if (!cell) {
			clip[0] = r->X + width -1 - table->Spacing;
			return NULL;
		}
		if (cell->DummyFor) {
			cell = cell->DummyFor;
		}
		if (x >= cell->c_OffsetX + cell->c_Width) {
			clip[0] = r->X + cell->c_OffsetX + cell->c_Width;
			clip[2] = clip[0] + table->Spacing;
			clip[1] = r->Y + cell->c_OffsetY - table->Spacing;
			clip[3] = clip[1] + cell->c_Height + table->Spacing *2;
			cell = NULL;
		} else if (y >= cell->c_OffsetY + cell->c_Height) {
			clip[0] = r->X + cell->c_OffsetX - table->Spacing;
			clip[2] = clip[0] + cell->c_Width + table->Spacing *2;
			clip[1] = r->Y + cell->c_OffsetY + cell->c_Height;
			clip[3] = clip[1] + table->Spacing;
			cell = NULL;
		}
	}
	return (cell ? &cell->Box : NULL);
}


/*----------------------------------------------------------------------------*/
static void
vTab_draw (DOMBOX * This, long x, long y, const GRECT * clip, void * highlight)
{
	TABLE table       = ((PARAGRPH)This)->Table;
	long  clip_bottom = clip->g_y + clip->g_h -1;
	long  row_y       = y + table->t_BorderW + table->Spacing;
	TAB_ROW row       = table->Rows;

	/*puts("table_draw");*/

	while (row) {
		TAB_CELL cell = row->Cells;
		long clip_top = (long)clip->g_y - row_y;

		while (cell) {
			if (cell->Box.ChildBeg && (cell->c_Height > clip_top)) {
				dombox_draw (&cell->Box, x + cell->c_OffsetX,
				             y + cell->c_OffsetY, clip, highlight);
			}
			cell = cell->RightCell;
		}

		if ((row_y += row->Height + table->Spacing) > clip_bottom)
			break;

		row = row->NextRow;
	}
}
