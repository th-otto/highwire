/* @(#)highwire/defs.h
 */
#define _HIGHWIRE_MAJOR_     0
#define _HIGHWIRE_MINOR_     1
#define _HIGHWIRE_REVISION_  1
#define _HIGHWIRE_BETATAG_   "alpha"
#define _HIGHWIRE_VERSION_   "0.1.2"

#include "hw-types.h"   /* get base definitions */

#define _HIGHWIRE_FULLNAME_  "HighWire"
#define _HIGHWIRE_RSC_       "highwire.rsc"
#define _HIGHWIRE_CFG_       "highwire.cfg"

#define _HW_ALERT(text) "[3][" _HIGHWIRE_FULLNAME_ " " text " ][ Exit ]"
#define _ERROR_SPEEDO_   _HW_ALERT("needs SpeedoGDOS |or NVDI � 3!")
#define _ERROR_NOMEM_    _HW_ALERT("got no GLOBAL memory!")
#define _ERROR_NORSC_    _HW_ALERT("cannot load |" _HIGHWIRE_RSC_ "!")


#ifdef _HW_NO_GUI
# define _HIGHWIRE_INFOLINE_  FALSE /* switch for the window infoline */
# define _HIGHWIRE_ENCMENU_   FALSE /* show encoding in menu */
# define _HIGHWIRE_RPOP_      FALSE /* switch for the right mouse popup */
# undef  GEM_MENU

#else
# define _HIGHWIRE_INFOLINE_  TRUE /* switch for the window infoline */
# define _HIGHWIRE_ENCMENU_   TRUE /* show encoding in menu */
# define _HIGHWIRE_RPOP_      TRUE /* switch for the right mouse popup */
# define  GEM_MENU
#endif


/* ******* Type Definitions ********* */

typedef enum {
	PAR_NONE = 0, PAR_LI, PAR_IMG, PAR_HR, PAR_TABLE
} PARAGRAPH_CODE;

typedef enum {
	ALN_LEFT = 0, ALN_JUSTIFY, ALN_CENTER, ALN_RIGHT
	#define ALN_NO_FLT ALN_JUSTIFY /* justify is impossible for floating */
} H_ALIGN;

typedef enum {
	ALN_TOP = 0, ALN_MIDDLE, ALN_BOTTOM, ALN_BASELINE,
	ALN_ABOVE, ALN_BELOW   /* what are these both for?  Sub & Sup */
} V_ALIGN;

typedef enum {
	BRK_NONE = 0,
	BRK_LN = 0x01, BRK_LEFT = 0x11, BRK_RIGHT = 0x21, BRK_ALL = 0x31
} L_BRK;

typedef enum {
	SCROLL_AUTO = 0, SCROLL_ALWAYS, SCROLL_NEVER
} SCROLL_CODE;

typedef enum {
	METH_GET = 0, METH_POST
} FORM_METHOD;


typedef enum {disc,square,circle,Number,alpha,Alpha,roman,Roman} BULLET;

/* http://www.indigo.ie/egt/standards/iso639/iso639-2-en.html
 * http://selfhtml.teamone.de/diverses/sprachenkuerzel.html
 * http://lcweb.loc.gov/standards/iso639-2/codechanges.html
 */
typedef enum {
	LANG_Unknown = 0,
	LANG_CA = ('c' << 8) + 'a',  /* Catalan, Katalanisch */
	LANG_CS = ('c' << 8) + 's',  /* Czech, Tschechisch */
	LANG_DA = ('d' << 8) + 'a',  /* Danish, D�nisch */
	LANG_DE = ('d' << 8) + 'e',  /* German, Deutsch */
	LANG_EL = ('e' << 8) + 'l',  /* Greek, Griechisch */
	LANG_EN = ('e' << 8) + 'n',  /* English, Englisch */
	LANG_ES = ('e' << 8) + 's',  /* Spanish, Spanisch */
	LANG_ET = ('e' << 8) + 't',  /* Estonian, Estnisch */
	LANG_FI = ('f' << 8) + 'i',  /* Finnish, Finnisch */
	LANG_FR = ('f' << 8) + 'r',  /* French, Franz�sisch */
	LANG_FY = ('f' << 8) + 'y',  /* Frisian, Friesisch */
	LANG_HE = ('h' << 8) + 'e',  /* Hebrew, Hebr�isch */
	LANG_HR = ('h' << 8) + 'r',  /* Croatian, Kroatisch */
	LANG_HU = ('h' << 8) + 'u',  /* Hungarian, Ungarisch */
	LANG_IT = ('i' << 8) + 't',  /* Italian, Italienisch */
	LANG_JA = ('j' << 8) + 'a',  /* Japanese, Japanisch */
	LANG_KO = ('k' << 8) + 'o',  /* Korean, Koreanisch */
	LANG_NL = ('n' << 8) + 'l',  /* Dutch, Niederl�ndisch */
	LANG_NB = ('n' << 8) + 'b',  /* Bokm�l Norwergian, Alt-Norwegisch */
	LANG_NN = ('n' << 8) + 'n',  /* Nynorsk Norwergian, Neu-Norwegisch */
	LANG_NO = ('n' << 8) + 'o',  /* Norwergian, Norwegisch */
	LANG_PL = ('p' << 8) + 'l',  /* Polish, Polnisch */
	LANG_PT = ('p' << 8) + 't',  /* Portuguese, Portugiesisch */
	LANG_RO = ('r' << 8) + 'o',  /* Rumanian, Rum�nisch */
	LANG_RU = ('r' << 8) + 'u',  /* Russian, Russisch */
	LANG_SH = ('s' << 8) + 'h',  /* Serbo-Croatian, Serbokroatisch, deprecated! */
	LANG_SK = ('s' << 8) + 'k',  /* Slovak, Slowakisch */
	LANG_SL = ('s' << 8) + 'l',  /* Slovenian, Slowenisch */
	LANG_SQ = ('s' << 8) + 'q',  /* Albanian, Albanisch */
	LANG_SR = ('s' << 8) + 'r',  /* Serbian, Serbisch */
	LANG_SV = ('s' << 8) + 'v',  /* Swedish, Schwedisch */
	LANG_TR = ('t' << 8) + 'r',  /* Turkish, T�rkisch */
	LANG_YI = ('y' << 8) + 'i',  /* Yiddish, Jiddisch */
	LANG_ZH = ('z' << 8) + 'h'   /* Chinese, Chinesisch */
} LANGUAGE;


/***** Forward Declarations *****/

typedef struct s_table_stack  * TBLSTACK;   /* in Table.c */
typedef struct s_table        * TABLE;      /* in Table.c */
typedef struct named_location * ANCHOR;
typedef struct frame_item     * FRAME;
typedef struct paragraph_item * PARAGRPH;
typedef struct word_item      * WORDITEM;
typedef struct s_font         * FONT;       /* in fontbase.h */
typedef struct s_location     * LOCATION;   /* in Location.h */
#if !defined(__PUREC__)
typedef struct s_containr     * CONTAINR;
typedef struct s_history      * HISTORY;
typedef struct s_parser       * PARSER;
typedef struct s_form         * FORM;
typedef struct s_input        * INPUT;
#else
# ifndef  CONTAINR
#  define CONTAINR void*
# endif
# ifndef  HISTORY
#  define HISTORY  void*
# endif
# ifndef  PARSER
#  define PARSER   void*
# endif
# ifndef  FORM
#  define FORM     void*
# endif
# ifndef  INPUT
#  define INPUT    void*
# endif
#endif


#ifndef MagX_cookie
#define 	MagX_cookie		0x4D616758L
#endif

#ifndef AES_single
#define	AES_single	0
#define	AES_MagiC	1
#define	AES_Geneva	2
#define	AES_MTOS	3
#define	AES_nAES	4
#endif

#ifndef ck_entry 
typedef struct {
    long cktag;
    long ckvalue;
} ck_entry;
#endif

#ifndef GLOBAL
#define GLOBAL 0x0020
#define ALLOCMODE 3|GLOBAL
#endif


/* ********** Consts ************** */

#define SPACE_BICS  561
#define SPACE_ATARI ' '
#define SPACE_UNI   0x0020 /* ' ' */

#define NOBRK_BICS  560
#define NOBRK_ATARI (0x100|SPACE_ATARI)
#define NOBRK_UNI   0x00A0

#define normal_font 0
#define header_font 1
#define pre_font 2
#define scroll_bar_width 15
#define scroll_step 24
#define DOMAIN_MINT 1
#define HW_PATH_MAX 260  /* at least VFAT standard 260 */


/* ********** Structures ***************** */

typedef struct {
	WORD handle;
	WORD dev_id;
	WORD wchar;
	WORD hchar;
	WORD wbox;
	WORD hbox;
	WORD xres;
	WORD yres;
	WORD noscale;
	WORD wpixel;
	WORD hpixel;
	WORD cheights;
	WORD linetypes;
	WORD linewidths;
	WORD markertypes;
	WORD markersizes;
	WORD faces;
	WORD patterns;
	WORD hatches;
	WORD colors;
	WORD ngdps;
	WORD cangdps[10];
	WORD gdpattr[10];
	WORD cancolour;
	WORD cantextrot;
	WORD canfillarea;
	WORD cancellarray;
	WORD palette;
	WORD locators;
	WORD valuators;
	WORD choicedevs;
	WORD stringdevs;
	WORD wstype;
	WORD minwchar;
	WORD minhchar;
	WORD maxwchar;
	WORD maxhchar;
	WORD minwline;
	WORD zero5;
	WORD maxwline;
	WORD zero7;
	WORD minwmark;
	WORD minhmark;
	WORD maxwmark;
	WORD maxhmark;
	WORD screentype;
	WORD bgcolors;
	WORD textfx;
	WORD canscale;
	WORD planes;
	WORD lut;
	WORD rops;
	WORD cancontourfill;
	WORD textrot;
	WORD writemodes;
	WORD inputmodes;
	WORD textalign;
	WORD inking;
	WORD rubberbanding;
	WORD maxvertices;
	WORD maxintin;
	WORD mousebuttons;
	WORD widestyles;
	WORD widemodes;
	WORD clipflag;  /* GEM 2, NVDI */
	WORD pixelm;  /* GEM 3 */
	WORD wpixelm;  /* GEM 3 */
	WORD hpixelm;  /* GEM 3 */
	WORD hpixelperinch;  /* GEM 3 */
	WORD vpixelperinch;  /* GEM 3 */
	WORD prnrot;  /* GEM 3 */
	void *quarter;  /* GEM 3 */
	WORD flags;  /* GEM 3, NVDI */
	WORD zero8;
	WORD flags2;  /* NVDI 5 */
	WORD zero9[14];
	WORD clipx;  /* undocumented! */
	WORD clipy;  /* undocumented! */
	WORD clipw;  /* undocumented! */
	WORD cliph;  /* undocumented! */
	WORD reserved[8];
} VDI_Workstation;


/* Modified size to st_size to eliminate one #ifdef in loader.c - Baldrick */
struct xattr {
	UWORD mode;
	long index;
	UWORD dev;
	UWORD reserved1;
	UWORD nlink;
	UWORD uid;
	UWORD gid;
	unsigned long st_size;
	long blksize;
	long nblocks;
	UWORD mtime;
	UWORD mdate;
	UWORD atime;
	UWORD adate;
	UWORD ctime;
	UWORD cdate;
	WORD attr;
	WORD reserved2;
	long reserved3;
	long reserved4;
};

/* ************ Parsing Constructs ************************ */

typedef struct long_offset {
	struct long_offset * Origin;
	long                 X, Y;
} OFFSET;

struct s_img_data {
	/* MFDB */
	void * fd_addr;
	WORD   fd_w, fd_h;
	WORD   fd_wdwidth;
	WORD   fd_stand;
	WORD   fd_nplanes;
	WORD   fd_r1, fd_r2, fd_r3;
	/*---*/
	UWORD  img_w, img_h; /* original extents           */
	WORD   fgnd,  bgnd;  /* colors for due-chrome data */
	size_t mem_size;     /* additional size, of data   */
};
typedef const struct s_img_data * cIMGDATA;
typedef       struct s_img_data * pIMGDATA;

typedef struct s_image {
	union {
		void   * Mfdb;
		cIMGDATA Data;
	}        u;
	WORDITEM word;
	PARAGRPH paragraph;
	FRAME    frame;
	LOCATION source;
	OFFSET offset;
	short  backgnd;
	short  set_w,  set_h;
	short  disp_w, disp_h;
	short  alt_w,  alt_h;
	short  vspace;
	short  hspace;
} * IMAGE;

struct font_step {
	WORD step;  /* HTML font size 0..7 */
	WORD colour;
	struct font_step *previous_font_step;
};

struct font_style {
	unsigned italic;
	unsigned bold;
	unsigned underlined;
	unsigned strike;
};

typedef union {
	unsigned long packed;
	struct {
		unsigned _pad1  :8;    /* struct alignment, always 0 */
		unsigned color  :8;
		unsigned undrln :1; /* underlined     */
		unsigned _pad2  :1;    /* struct alignment, always 0 */
		unsigned strike :1; /* strike through */
		unsigned condns :1; /* condensed      */
		unsigned italic :1;
		unsigned bold   :1;
		unsigned font   :2;
		unsigned size   :8;
	} s;
} TEXTATTR;
#define TEXTATTR(s,c) (((long)(c & 0xFF) <<16) | (s & 0xFF))
                      /* pseudo constructor for text size and colour */
#define TA_SizeMASK 0x000000FFuL
#define TA_FaceMASK 0x00001F00uL
#define TA_AttrMASK 0x0000A000uL
#define TA_ColrMASK 0x00FF0000uL
#define TA_ALL_MASK (TA_SizeMASK|TA_FaceMASK|TA_AttrMASK|TA_ColrMASK)

#ifdef __LATICE__
# define _TAsplit(a,n) (((unsigned char *)&(a).packed)[n])
# define TA_Color(a)      _TAsplit(a,1)
# define TAsetUndrln(a,b) {if(b) _TAsplit(a,2)|=0x80; else _TAsplit(a,2)&=0x7F;}
# define TAgetUndrln(a)   ((_TAsplit(a,2) & 0x80) ? 8 :0)
# define TAsetStrike(a,b) {if(b) _TAsplit(a,2)|=0x20; else _TAsplit(a,2)&=0xDF;}
# define TAgetStrike(a)   ((_TAsplit(a,2) & 0x20) ? 1 :0)
# define TAsetCondns(a,b) {if(b) _TAsplit(a,2)|=0x10; else _TAsplit(a,2)&=0xEF;}
# define TAgetCondns(a)   ((_TAsplit(a,2) & 0x40) ? 4 :0)
# define TAsetItalic(a,b) {if(b) _TAsplit(a,2)|=0x08; else _TAsplit(a,2)&=0xF7;}
# define TAgetItalic(a)   ((_TAsplit(a,2) & 0x08) ? 1 :0)
# define TAsetBold(a,b)   {if(b) _TAsplit(a,2)|=0x04; else _TAsplit(a,2)&=0xFB;}
# define TAgetBold(a)     ((_TAsplit(a,2) & 0x04) ? 1 :0)
# define TAsetFont(a,f)   {_TAsplit(a,2) = (_TAsplit(a,2) & 0xFC) | (f & 0x03);}
# define TAgetFont(a)     (_TAsplit(a,2) & 0x03)
# define TA_Size(a)       _TAsplit(a,3)
#else
# define TA_Color(a)      ((a).s.color)
# define TAsetUndrln(a,b) {(a).s.undrln = (b ? 1 : 0);}
# define TAgetUndrln(a)   ((a).s.undrln ? 8 :0)
# define TAsetStrike(a,b) {(a).s.strike = (b ? 1 : 0);}
# define TAgetStrike(a)   ((a).s.strike ? 1 :0)
# define TAsetCondns(a,b) {(a).s.condns = (b ? 1 : 0);}
# define TAgetCondns(a)   ((a).s.condns ? 1 :0)
# define TAsetItalic(a,b) {(a).s.italic = (b ? 1 : 0);}
# define TAgetItalic(a)   ((a).s.italic ? 1 :0)
# define TAsetBold(a,b)   {(a).s.bold = (b ? 1 : 0);}
# define TAgetBold(a)     ((a).s.bold ? 1 :0)
# define TAsetFont(a,f)   {(a).s.font = f;}
# define TAgetFont(a)     ((a).s.font)
# define TA_Size(a)       ((a).s.size)
#endif
#define TAgetFace(a) (((short)(a).packed & 0x0F00) >>8)

typedef struct word_line {
	PARAGRPH Paragraph;  /* back reference */
	long     OffsetY;    /* distance of the baseline to the paragraph's top */
	WORDITEM Word;    /* the first word of the line */
	short    Count;   /* number of words */
	short    Ascend;  /* space above the baseline */
	short    Descend; /* space below the baseline */
	long     Width;
	struct word_line * NextLine;
} * WORDLINE;

struct word_item {
	const UWORD _priv; /* * for internal use, don't touch! */
	UWORD     length;  /* number of 16-bit characters in item */
	WCHAR  * item;
	TEXTATTR attr;
	FONT     font;
	WORD h_offset;
	WORD word_width;
	WORD word_height;
	WORD word_tail_drop;
	WORD space_width;
	BOOL wrap;
	L_BRK   line_brk;
	V_ALIGN vertical_align;
	struct url_link  * link;
	IMAGE    image;
	INPUT    input;
	WORDLINE line;  /* back reference */
	WORDITEM next_word;
};

struct paragraph_item {
	WORDITEM item;
	TABLE    Table;
	WORDLINE Line;    /* list of word lines */
	WORD     Indent;  /* horizontal offset to the left side */
	WORD     Rindent; /* indent of the right side */
	WORD     Hanging; /* <0: left, >0: right hanging */
	short    Backgnd; /* colour or -1 */
	OFFSET   Offset;
	long     Width;
	long     Height;
	PARAGRAPH_CODE paragraph_code;
	H_ALIGN alignment;
	H_ALIGN floating;
	WORD eop_space;
	long min_width;
	long max_width;
	PARAGRPH next_paragraph;
};

typedef struct {
	PARAGRPH Item;
	long     Minimum; /* smallest width where the content fits in    */
	long     Width;   /* set Value, must not be smaller than Minimum */
	long     Height;  /* calculated value resulting from given Width */
	short    Backgnd; /* -1 or colour value                          */
	H_ALIGN  Alignment;
	short    MarginTop, MarginBot; /* top and bottom margins */
	short    MarginLft, MarginRgt; /* left and right margins */
} CONTENT;

struct frame_item {
	CONTAINR Container;
	LOCATION Location;
	ENCODING Encoding;
	BOOL     ForceEnc; /* don't use encoding from meta tag */
	LANGUAGE Language;
	UWORD    MimeType; /* determines whether the content needs no parsing */
	CONTENT  Page;
	struct slider {
		long scroll;
		BOOL on;
		WORD lu, rd; /* left/up, right/down arrow borders */
		WORD pos;    /* slider position */
		WORD size;   /* slider size     */
	}    v_bar, h_bar;
	BOOL border;
	BOOL resize;
	SCROLL_CODE scroll;
	WORD   text_colour;
	WORD   link_colour;
	char * base_target;
	GRECT  clip;
	ANCHOR first_named_location;
	FORM   FormList;        /* FORM list for user interaction */
};

typedef struct list_stack_item * LSTSTACK;

typedef struct parse_sub {
	LSTSTACK           lst_stack;
	TBLSTACK           tbl_stack;
	void             * form;
	LANGUAGE           quot_lang; /* language used for quotation marks */
	short              backgnd;
	struct font_style  styles;
	PARAGRPH           paragraph;
	PARAGRPH           prev_par;
	WORDITEM           word;      /* the item actually to process */
	WORDITEM           prev_wrd;  /* <br> tags affect this        */
	UWORD              nowrap;
	WORD               font_size;
	struct font_step * font_step;
	ANCHOR           * anchor;
	WCHAR            * text;  /* points behind the last stored character */
	WCHAR            * buffer;
} PARSESUB;
typedef struct parse_sub * TEXTBUFF;

struct list_stack_item {
	BULLET   BulletStyle;
	WORD     Counter;
	short    Indent;  /* horizontal offset to the left side */
	short    Hanging; /* left shift value for <li> bullet/number */
	WORDITEM Spacer;  /* item just befor the bullet/number of actual <li> line */
	LSTSTACK next_stack_item;
};

/* ******************** Action Constructs ************************* */

struct named_location {
	OFFSET offset;
	const char * address;
	struct named_location *next_location;
};

struct url_link {
	BOOL     isHref;
	char   * address;
	WORDITEM start;   /* where this link is reffered the first time */
	union {
		ANCHOR anchor; /* if isHref == FALSE */
		char * target; /* if isHref == TRUE  */
	} u;
	ENCODING encoding;
};


#ifdef __PUREC__
/* Pure C 1.1 reports errors as "Undefined structure 's_table[_stack]'"!
 * So let the compiler know the structures. */
#include "Table.h"
#include "Location.h"
#include "fontbase.h"
#endif /* __PUREC__ */
