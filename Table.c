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
#include "Table.h"

#define t_Width      Paragraph->Width
#define t_Height     Paragraph->Height
#define t_MinWidth   Paragraph->min_width
#define t_MaxWidth   Paragraph->max_width

#ifdef DEBUG
	#define _DEBUG
#else
/*	#define _DEBUG */
#endif


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
				if (!cell->DummyFor) {
					content_destroy (&cell->Content);
				}
				free (cell);
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
table_start (PARSER parser)
{
	char     buf[10];
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
	stack->SavedCurr = *current;
	stack->Backgnd   = current->backgnd;
	stack->_debug = 'A';
	
	par->Table          = table;
	par->paragraph_code = PAR_TABLE;
	par->Offset.Origin  = (current->tbl_stack && current->tbl_stack->WorkCell
	                       ? &current->tbl_stack->WorkCell->Offset : NULL);
	if (get_value (parser, KEY_ALIGN, buf, sizeof(buf))) {
		if      (stricmp (buf, "right")  == 0) par->floating = ALN_RIGHT;
		else if (stricmp (buf, "center") == 0) par->floating = ALN_CENTER;
		else if (stricmp (buf, "left")   == 0) par->floating = ALN_LEFT;
	}
	
	current->tbl_stack = stack;
	current->lst_stack = NULL;
	current->font_step = new_step (3, parser->Frame->text_color);

	table->Paragraph = par;
	table->Rows      = NULL;
	table->NumCols   = 0;
	table->FixCols   = 0;
	table->NumRows   = 0;
	table->Minimum   = NULL;
	table->Maximum   = NULL;
	table->Percent   = NULL;
	table->ColWidth  = NULL;

	table->Color   = get_value_color (parser, KEY_BGCOLOR);
	table->Border  = get_value_unum  (parser, KEY_BORDER, -1);
	if (table->Border < 0) {
		table->Border = (get_value (parser, KEY_BORDER, NULL, 0) ? 1 : 0);
	}
	table->Spacing = get_value_unum (parser, KEY_CELLSPACING, 2);
	table->Padding = get_value_unum (parser, KEY_CELLPADDING, 1)
	               + (table->Border ? 1 : 0);
	table->SetWidth   = get_value_size (parser, KEY_WIDTH);
	table->SetHeight  = get_value_size (parser, KEY_HEIGHT);
	table->t_MinWidth = table->t_MaxWidth = table->Border *2 + table->Spacing;
}


/*============================================================================*/
void
table_row (PARSER parser, BOOL beginNend)
{
	TBLSTACK stack = parser->Current.tbl_stack;
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
		char    buf[10];
		if (!row) {
			row = malloc (sizeof (struct s_table_row));
			row->Cells   = NULL;
			row->NextRow = NULL;
			table->NumRows++;
		}
		row->Color     = get_value_color (parser, KEY_BGCOLOR);
		row->MinHeight = get_value_size  (parser, KEY_HEIGHT);
		row->Height    = table->Padding *2;
	
		stack->AlignH = ALN_LEFT;
		if (get_value (parser, KEY_ALIGN, buf, sizeof(buf))) {
			if      (stricmp (buf, "right")   == 0) stack->AlignH = ALN_RIGHT;
			else if (stricmp (buf, "center")  == 0) stack->AlignH = ALN_CENTER;
			else if (stricmp (buf, "justify") == 0) stack->AlignH = ALN_JUSTIFY;
		}

		stack->AlignV = ALN_MIDDLE;
		if (get_value (parser, KEY_VALIGN, buf, sizeof(buf))) {
			if      (stricmp (buf, "top")   == 0)    stack->AlignV = ALN_TOP;
			else if (stricmp (buf, "bottom")  == 0)  stack->AlignV = ALN_BOTTOM;
		/*	else if (stricmp (buf, "baseline") == 0) stack->AlignV = ALN_BASELINE;*/
		}
	
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
new_cell (TAB_CELL left_side, short padding)
{
	TAB_CELL cell = malloc (sizeof (struct s_table_cell));

	if (left_side) {
		left_side->RightCell = cell;
	}
	content_setup (&cell->Content, NULL, padding, -1);
	cell->ColSpan   = 1;
	cell->RowSpan   = 1;
	cell->DummyFor  = NULL;
	cell->RightCell = NULL;
	cell->BelowCell = NULL;
	cell->Offset.Origin = NULL;

	cell->_debug = '.';

	return cell;
}


/*============================================================================*/
void
table_cell (PARSER parser, BOOL is_head)
{
	TBLSTACK stack = parser->Current.tbl_stack;
	TABLE    table = stack->Table;
	TAB_ROW  row;
	TAB_CELL cell;
	short    span;
	char     buf[10];

/*	printf("table_cell('%c')\n", stack->_debug);*/
	
	if (stack->WorkCell) {
		paragrph_finish (&parser->Current);
		parser->Current.font_step = new_step (3, parser->Frame->text_color);
	}
	
	if (!stack->WorkRow) {
		table_row (parser, TRUE);
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
				prev = stack->PrevRow->Cells = new_cell (NULL, table->Padding);
			}
			cell = NULL;
			do {
				prev->BelowCell = cell = new_cell (cell, table->Padding);
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
		cell = new_cell (stack->WorkCell, table->Padding);
		if (!row->Cells) {
			row->Cells = cell;
		}
	}
	stack->WorkCell = cell;

	cell->Content.Item = new_paragraph (&parser->Current);
	cell->Offset.Origin               = &table->Paragraph->Offset;
	cell->Content.Item->Offset.Origin = &cell->Offset;
	if (is_head) {
		word_set_bold (&parser->Current, TRUE);
	}
	cell->_debug = stack->_debug++;

	#define par cell->Content.Item
	par->alignment = (is_head ? ALN_CENTER : stack->AlignH);

	parser->Current.nowrap = get_value (parser, KEY_NOWRAP, NULL,0);
	
	if (get_value (parser, KEY_ALIGN, buf, sizeof(buf))) {
		if      (stricmp (buf, "left")    == 0) par->alignment = ALN_LEFT;
		else if (stricmp (buf, "right")   == 0) par->alignment = ALN_RIGHT;
		else if (stricmp (buf, "center")  == 0) par->alignment = ALN_CENTER;
		else if (stricmp (buf, "justify") == 0) par->alignment = ALN_JUSTIFY;
	}
	cell->Content.Alignment = par->alignment;
	#undef par

	cell->AlignV = stack->AlignV;
	if (get_value (parser, KEY_VALIGN, buf, sizeof(buf))) {
		if      (stricmp (buf, "top")   == 0)    cell->AlignV = ALN_TOP;
		else if (stricmp (buf, "bottom")  == 0)  cell->AlignV = ALN_BOTTOM;
	/*	else if (stricmp (buf, "baseline") == 0) cell->AlignV = ALN_BASELINE;*/
	}

	if ((cell->Content.Width  = get_value_size (parser, KEY_WIDTH))  > 1024) {
		  cell->Content.Width  = 0; /*1024;*/
	}
	if ((cell->Content.Height = get_value_size (parser, KEY_HEIGHT)) > 1024) {
		  cell->Content.Height = 1024;
	}
	
	if ((cell->Content.Backgnd = get_value_color (parser, KEY_BGCOLOR)) < 0) {
		if      (row->Color   >= 0) cell->Content.Backgnd = row->Color;
		else if (table->Color >= 0) cell->Content.Backgnd = table->Color;
		else                        cell->Content.Backgnd = stack->Backgnd;
	}
	parser->Current.backgnd = cell->Content.Backgnd;
	
	if ((span = get_value_unum (parser, KEY_ROWSPAN, 0)) > 1) {
		cell->RowSpan = span;
	}
	if ((span = get_value_unum (parser, KEY_COLSPAN, 0)) > 1) {
		cell->ColSpan = span;
		span = 2 - span;
		do {
			if (!stack->WorkCell->RightCell) {
				stack->WorkCell = new_cell (stack->WorkCell, table->Padding);
			} else {
				stack->WorkCell = stack->WorkCell->RightCell;
			}
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
			TAB_CELL next = new_cell (last, table->Padding);
			while (!prev->RightCell) {
				next = next->BelowCell = new_cell (prev, table->Padding);
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
		table_row (parser, FALSE);
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
	
	table->t_MinWidth =
	table->t_MaxWidth = table->Border *2 + table->Spacing * (table->NumCols +1);
	
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
			printf ("%c ", (cell->DummyFor ? tolower (cell->DummyFor->_debug) : cell->_debug));
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
			if (cell->Content.Item) {
				if (cell->Content.Width < 0 && table->SetWidth > 0) {
					short width = table->SetWidth - table->t_MinWidth;
					cell->Content.Width = (-cell->Content.Width * width +512) /1024;
					if (cell->Content.Width <= 0) cell->Content.Width = 1;
				}
				if (cell->ColSpan == 1) {
					long width = content_minimum (&cell->Content);
					if (width <= padding) {
						cell->Content.Width = 0;
					}
					if (cell->Content.Width > 0) {
						if (width < cell->Content.Width) width = cell->Content.Width;
						if (width < *minimum)    width = *minimum;
						if (width < *fixed)      width = *fixed;
						*minimum = *fixed = width;
					} else {
						if (*minimum < width) *minimum = width;
						width = content_maximum (&cell->Content);
						if (*maximum < width) *maximum = width;
						if (cell->Content.Width < 0) {
							if (table->NumCols > 1) {
								*percent = -cell->Content.Width;
							} else {
								cell->Content.Width = 0;
							}
						}
					}
				}
				if (cell->RowSpan == 1) {
					if (row->MinHeight < cell->Content.Height) {
						row->MinHeight = cell->Content.Height;
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
			if (cell->Content.Item && cell->ColSpan > 1) {
				long width = content_minimum (&cell->Content);
				spread_width (minimum, cell->ColSpan, table->Spacing, width);
				if (cell->Content.Width > 0) {
					short empty = 0;
					if (width < cell->Content.Width) width = cell->Content.Width;
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
					              content_maximum (&cell->Content));
					if (cell->Content.Width < 0 && cell->ColSpan < table->NumCols) {
						short empty = 0;
						percent = table->Percent + (fixed - table->ColWidth);
						width   = -cell->Content.Width;
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
		table->t_MinWidth += *(minimum++);
		table->t_MaxWidth += *(maximum++);
	} while ((column = column->RightCell) != NULL);

	if (table->FixCols == table->NumCols) {
		table->SetWidth = table->t_MaxWidth = table->t_MinWidth;

	} else if (table->SetWidth > 0) {
		if (table->SetWidth < table->t_MinWidth) {
			 table->SetWidth = table->t_MinWidth;
		} else {
			short spread = table->SetWidth - table->t_MinWidth;
			table->t_MinWidth = table->SetWidth;
			table->SetWidth  += spread;
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
			if (cell->Content.Item && cell->RowSpan > 1) {
				short span   = cell->RowSpan -1;
				short height = cell->Content.Height - row->MinHeight;
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


/*============================================================================*/
long
table_calc (TABLE table, long max_width)
{
	TAB_ROW row;
	long    set_width;
	long    y = table->Border + table->Spacing;

	if (!table->NumCols) {
		#ifdef _DEBUG
			printf ("table_calc(): no columns!\n");
		#endif
		return table->t_Height;
	}
	/*printf ("table_calc(%li) %i\n", max_width, table->SetWidth);*/


	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	 * Step 1:  adjust the given width to the table's boundarys.
	 */
	if (table->SetWidth > 0) {
		set_width = table->SetWidth;
	} else if (table->SetWidth) {
		set_width = (max_width * -table->SetWidth + 512) / 1024;
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

	} else if (table->SetWidth) { /* - - - - - - - - - - - - - - - - - - - - -
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
		
		if (table->SetWidth > table->t_MinWidth) {
			col_width = table->ColWidth;
			minimum   = table->Minimum;
			i         = table->NumCols;
			do {
				*(minimum++) = *(col_width++);
			} while (--i);

			set_width = table->SetWidth = table->t_MinWidth;
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
		/* first mark all columns to be spread with a negative value */
		do {
			*col_width = *minimum;
			if (*percent) {
				long width = (table->t_MinWidth * *percent +512) /1024 - *minimum;
				if (width > spread) {
					*col_width += spread;
					spread = 0;
				} else if (width > 0) {
					*col_width += width;
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


	row = table->Rows;
	do {
		long   * col_width = table->ColWidth;
		TAB_CELL cell      = row->Cells;
		row->Height = row->MinHeight;
		while (cell) {
			long width = *(col_width++);
			if (cell->Content.Item) {
				short span = cell->ColSpan -1;
				while (span--) width += col_width[span] + table->Spacing;
				content_calc (&cell->Content, width);
				if (cell->RowSpan == 1 && row->Height < cell->Content.Height) {
					row->Height = cell->Content.Height;
				}
			} else {
				cell->Content.Width = width;
			}
			cell = cell->RightCell;
		}
	} while ((row = row->NextRow) != NULL);


	table->t_Height = table->Border *2 + table->Spacing;
	row = table->Rows;
	do {
		TAB_CELL cell = row->Cells;
		while (cell) {
			if (cell->Content.Item && cell->RowSpan > 1) {
				short span   = cell->RowSpan -1;
				short height = cell->Content.Height - row->Height;
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


	row = table->Rows;
	do {
		long   * col_width = table->ColWidth;
		TAB_CELL cell      = row->Cells;
		long     x         = table->Border + table->Spacing;
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
			if (height != cell->Content.Height) {
				content_stretch (&cell->Content, height, cell->AlignV);
			}
			cell->Offset.X = x;
			cell->Offset.Y = y;
			x += *(col_width++) + table->Spacing;
		} while ((cell = cell->RightCell) != NULL);
		y += row->Height + table->Spacing;
	} while ((row = row->NextRow) != NULL);

	return table->t_Height;
}


/*============================================================================*/
long
table_draw (TABLE table, short x, long y, const GRECT * clip, void * highlight)
{
	long clip_bottom = clip->g_y + clip->g_h -1;
	long row_y       = y;
	TAB_ROW row = table->Rows;

	/*puts("table_draw");*/

	if (table->Border) {
		GRECT b;
		b.g_x = x;
		b.g_y = y;
		b.g_w = table->t_Width;
		b.g_h = table->t_Height;
		draw_border (&b, G_WHITE, G_LBLACK, table->Border);
		row_y += table->Border;
	}
	if (table->Color >= 0) {
		PXY   p[2];
		short col_x = x + table->Border;
		p[1].p_x = (p[0].p_x = col_x) + table->t_Width  - table->Border *2 -1;
		p[1].p_y = (p[0].p_y = row_y) + table->t_Height - table->Border *2 -1;
		vsf_color (vdi_handle, table->Color);
		v_bar     (vdi_handle, (short*)p);
	}
	row_y += table->Spacing;

	while (row) {
		long   * width = table->ColWidth;
		short    col_x = x + table->Border + table->Spacing;
		TAB_CELL cell  = row->Cells;
		long clip_top  = (long)clip->g_y - row_y;

		while (cell) {
			if (cell->Content.Item && (cell->Content.Height > clip_top)) {
				draw_contents (&cell->Content, x + cell->Offset.X,
				               y + cell->Offset.Y, clip, highlight);
				if (table->Border) {
					GRECT b;
					b.g_x = col_x;
					b.g_y = row_y;
					b.g_w = cell->Content.Width;
					b.g_h = cell->Content.Height;
					draw_border (&b, G_LBLACK, G_WHITE, 1);
					vsl_color (vdi_handle, G_BLACK);
				}
			}
			col_x += *(width++) + table->Spacing;
			cell  =  cell->RightCell;
		}

		if ((row_y += row->Height + table->Spacing) > clip_bottom)
			break;

		row = row->NextRow;
	}

	return (table->t_Height);
}


/*============================================================================*/
CONTENT *
table_content (TABLE table, long x, long y, long area[4])
{
	CONTENT * cont   = NULL;
	short     border = table->Border + table->Spacing;
	TAB_ROW   row    = (table->NumCols ? table->Rows : NULL);
	
	if (row && border) {
		if        (y < border) {
			area[2] = table->t_Width;
			area[3] = border;
			row = NULL;
		} else if (x < border) {
			area[2] = border;
			area[3] = table->t_Height;
			row = NULL;
		} else if (x >= table->t_Width - border) {
			area[0] += table->t_Width - border;
			area[2] =  border;
			area[3] =  table->t_Height;
			row = NULL;
		} else if (y >= table->t_Height - border) {
			area[1] += table->t_Height - border;
			area[2] = table->t_Width;
			area[3] = border;
			row = NULL;
		
		} else {
			x -= border;
			y -= border;
		}	
	}
	if (row) {
		TAB_CELL cell = NULL;
		long     c_x  = border;
		long     c_y  = border;
		long     r_h  = 0;
		do {
			r_h = row->Height;
			if ((y -= r_h) < table->Spacing) {
				cell = row->Cells;
				break;
			} else {
				y   -=       table->Spacing;
				c_y += r_h + table->Spacing;
			}
		} while ((row = row->NextRow) != NULL);
		
		if (cell) {
			long * width = table->ColWidth;
			do {
				if ((x -= *width) < table->Spacing) {
					if (cell->ColSpan & 0xFFFE) x = -1;
					if (cell->RowSpan & 0xFFFE) y = -1;
					if (cell->DummyFor) cell = cell->DummyFor;
					if (x >= 0) c_x += *width;
					break;
				} else {
					x   -=              table->Spacing;
					c_x += *(width++) + table->Spacing;
				}
			} while ((cell = cell->RightCell) != NULL);
		}
		
		if (x >= 0) {
			area[0] += c_x;
			area[2] =  table->Spacing;
			if (cell) {
				area[1] += cell->Offset.Y       - table->Spacing;
				area[3] =  cell->Content.Height + table->Spacing *2;
			} else {
				area[1] += c_y - table->Spacing;
				area[3] =  r_h + table->Spacing *2;
			}
		} else if (y >= 0) {
			area[1] += c_y + r_h;
			area[3] =  table->Spacing;
			if (cell) {
				area[0] += cell->Offset.X      - table->Spacing;
				area[2] =  cell->Content.Width + table->Spacing *2;
			} else {
				area[0] += c_x;
				area[2] =  table->Spacing;
			}
		} else {
			area[0] += cell->Offset.X;
			area[1] += cell->Offset.Y;
			if (!cell->Content.Item) {
				area[0] -= table->Spacing;
				area[1] -= table->Spacing;
				area[2] =  cell->Content.Width  + table->Spacing *2;
				area[3] =  cell->Content.Height + table->Spacing *2;
			} else {
				cont = &cell->Content;
			}
		}
	}
	return cont;
}
