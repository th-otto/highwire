/* @(#)highwire/en_atari.h
 *
 * Translation Tables for ATARI System Encoded Fonts (*.FNT).
 *
*/


/*==============================================================================
 * Unicode
 *
 * With Style-Fonts and Atari standard TORG 105, U+20AC EURO SIGN is 236.  With
 * Milan TOS it is 222.  Here I used the "element of" symbol.
 * Rainer Seitel, 2002-03
 *
 * Format: {Unicode, zero to five Atari system font characters, 0}, 0
 * *3.2* before: new in Unicode 3.2
 *               http://www.unicode.org/unicode/reports/tr28/
 *               http://www.unicode.org/charts/PDF/U32-????.pdf
 * *HW* before: HighWire private use character
 */
static const WCHAR ATARI_MULTI_0x0085[] = {'.','.','.',0};
static const WCHAR ATARI_MULTI_0x0089[] = {0xF8,'/','o','o',0};
static const WCHAR ATARI_MULTI_0x00BE[] = {' ','3','/','4',0};
static const WCHAR ATARI_MULTI_0x09F3[] = {'I','N','R',0};
static const WCHAR ATARI_MULTI_0x0E3F[] = {'T','H','B',0};
static const WCHAR ATARI_MULTI_0x17DB[] = {'K','H','R',0};
static const WCHAR ATARI_MULTI_0x2026[] = {'.','.','.',0};
static const WCHAR ATARI_MULTI_0x2030[] = {0xF8,'/','o','o',0};
static const WCHAR ATARI_MULTI_0x20A7[] = {'P','t','s',0};
static const WCHAR ATARI_MULTI_0x20AA[] = {'I','L','S',0};
static const WCHAR ATARI_MULTI_0x2100[] = {'a','/','c',0};
static const WCHAR ATARI_MULTI_0x2101[] = {'a','/','s',0};
static const WCHAR ATARI_MULTI_0x2105[] = {'c','/','o',0};
static const WCHAR ATARI_MULTI_0x2106[] = {'c','/','u',0};
static const WCHAR ATARI_MULTI_0x2295[] = {'(','+',')',0};
static const WCHAR ATARI_MULTI_0x2297[] = {'(','x',')',0};
static const WCHAR ATARI_MULTI_0x2299[] = {'(',0xFA,')',0};
static const WCHAR ATARI_MULTI_0x22EF[] = {0xFA,0xFA,'.',0};
static const WCHAR ATARI_MULTI_0x2680[] = {'[',0xFA,']',0};
static const WCHAR ATARI_MULTI_0x2681[] = {'[',0xFA,0xFA,']',0};
static const WCHAR ATARI_MULTI_0x2682[] = {'[',0xFA,0xFA,0xFA,']',0};
static const WCHAR ATARI_MULTI_0x2683[] = {'[',':',':',']',0};
static const WCHAR ATARI_MULTI_0x2684[] = {'[',':',0xFA,':',']',0};
static const WCHAR ATARI_MULTI_0x2685[] = {'[',':',':',':',']',0};
static const WCHAR ATARI_MULTI_0xFB03[] = {'f','f','i',0};
static const WCHAR ATARI_MULTI_0xFB04[] = {'f','f','l',0};
#define SMALL(u,t,x) {u, t,     ((long)x +0) <<16}
#define MULTI(u)     {u, 0xFFFF, (long)ATARI_MULTI_##u}

static const struct s_uni_trans Unicode_to_Atari[] = {
SMALL(0x80,0xee,), SMALL(0x82,'\'',), SMALL(0x83,0x9f,), SMALL(0x84,'"',), MULTI(0x0085), 
SMALL(0x86,0xbb,), SMALL(0x87,0xbb,), SMALL(0x88,'^',), MULTI(0x0089), 
SMALL(0x8A,'S',), SMALL(0x8B,'<',), SMALL(0x8C,0xb5,), SMALL(0x8E,'Z',), SMALL(0x91,'\'',), 
SMALL(0x92,'\'',), SMALL(0x93,'"',), SMALL(0x94,'"',), SMALL(0x95,0xf9,), SMALL(0x96,'-',), SMALL(0x97,'-','-'), 
SMALL(0x98,'~',), SMALL(0x99,0xbf,), SMALL(0x9A,'s',), SMALL(0x9B,'>',), SMALL(0x9C,0xB4,), 
SMALL(0x9E,'z',), SMALL(0x9F,'Y',), 
SMALL(0x00A0,NOBRK_ATARI,), SMALL(0x00A1,0xad,), SMALL(0x00A2,0x9b,), SMALL(0x00A3,0x9c,), SMALL(0x00A5,0x9d,), 
SMALL(0x00A6,17,), SMALL(0x00A7,0xDD,), SMALL(0x00A8,0xB9,), SMALL(0x00A9,0xBD,), SMALL(0x00AA,0xA6,), 
SMALL(0x00AB,0xAE,), SMALL(0x00AC,0xAA,), SMALL(0x00AD,'-',), SMALL(0x00AE,0xBE,), SMALL(0x00AF,0xFF,), 
SMALL(0x00B0,0xF8,), SMALL(0x00B1,0xF1,), SMALL(0x00B2,0xFD,), SMALL(0x00B3,0xFE,), SMALL(0x00B4,0xBA,), 
SMALL(0x00B5,0xE6,), SMALL(0x00B6,0xBC,), SMALL(0x00B7,0xFA,), SMALL(0x00B9,'\'',), SMALL(0x00BA,0xA7,), 
SMALL(0x00BB,0xAF,), SMALL(0x00BC,0xAC,), SMALL(0x00BD,0xAB,), MULTI(0x00BE), 
SMALL(0x00BF,0xA8,), SMALL(0x00C0,0xB6,), SMALL(0x00C1,'A',), SMALL(0x00C2,'A',), SMALL(0x00C3,0xB7,), 
SMALL(0x00C4,0x8E,), SMALL(0x00C5,0x8F,), SMALL(0x00C6,0x92,), SMALL(0x00C7,0x80,), SMALL(0x00C8,'E',), 
SMALL(0x00C9,0x90,), SMALL(0x00CA,'E',), SMALL(0x00CB,'E',), SMALL(0x00CC,'I',), 
SMALL(0x00CD,'I',), SMALL(0x00CE,'I',), SMALL(0x00CF,'I',), SMALL(0x00D0,'D',), 
SMALL(0x00D1,0xA5,), SMALL(0x00D2,'O',), SMALL(0x00D3,'O',), SMALL(0x00D4,'O',), 
SMALL(0x00D5,0xB8,), SMALL(0x00D6,0x99,), SMALL(0x00D7,'x',), SMALL(0x00D8,0xB2,), SMALL(0x00D9,'U',), 
SMALL(0x00DA,'U',), SMALL(0x00DB,'U',), SMALL(0x00DC,0x9a,), SMALL(0x00DD,'Y',), 
SMALL(0x00DF,0x9E,), SMALL(0x00E0,0x85,), SMALL(0x00E1,0xA0,), SMALL(0x00E2,0x83,), SMALL(0x00E3,0xB0,), 
SMALL(0x00E4,0x84,), SMALL(0x00E5,0x86,), SMALL(0x00E6,0x91,), SMALL(0x00E7,0x87,), SMALL(0x00E8,0x8A,), 
SMALL(0x00E9,0x82,), SMALL(0x00EA,0x88,), SMALL(0x00EB,0x89,), SMALL(0x00EC,0x8D,), SMALL(0x00ED,0xA1,), 
SMALL(0x00EE,0x8C,), SMALL(0x00EF,0x8B,), SMALL(0x00F0,0xEB,), SMALL(0x00F1,0xA4,), SMALL(0x00F2,0x95,), 
SMALL(0x00F3,0xA2,), SMALL(0x00F4,0x93,), SMALL(0x00F5,0xB1,), SMALL(0x00F6,0x94,), SMALL(0x00F7,0xF6,), 
SMALL(0x00F8,0xB3,), SMALL(0x00F9,0x97,), SMALL(0x00FA,0xA3,), SMALL(0x00FB,0x96,), SMALL(0x00FC,0x81,), 
SMALL(0x00FD,'y',), SMALL(0x00FF,0x98,), SMALL(0x0108,'C','h'), SMALL(0x0109,'c','h'), 
SMALL(0x010C,'C',), SMALL(0x010D,'c',), SMALL(0x010F,'d','\''), SMALL(0x011C,'G','h'), 
SMALL(0x011D,'g','h'), SMALL(0x0124,'H','h'), SMALL(0x0125,'h','h'), 
SMALL(0x0132,0xC1,), SMALL(0x0133,0xC0,), SMALL(0x0134,'J','h'), SMALL(0x0135,'j','h'), 
SMALL(0x013E,'l','\''), SMALL(0x013F,'L',0xFA), SMALL(0x0140,'l',0xFA), SMALL(0x0149,'\'','n'), 
SMALL(0x0152,0xB5,), SMALL(0x0153,0xB4,), SMALL(0x015C,'S','h'), SMALL(0x015D,'s','h'), 
SMALL(0x0160,'S',), SMALL(0x0161,'s',), SMALL(0x016D,'u',), SMALL(0x0178,'Y',), SMALL(0x017D,'Z',), 
SMALL(0x017E,'z',), SMALL(0x017F,0xF4,), SMALL(0x01C0,'|',), SMALL(0x01C1,'|','|'), SMALL(0x01C3,'!',), 
SMALL(0x01C7,'L','J'), SMALL(0x01C8,'L','j'), SMALL(0x01C9,'l','j'), SMALL(0x01CA,'N','J'), 
SMALL(0x01CB,'N','j'), SMALL(0x01CC,'n','j'), SMALL(0x01DD,26,), SMALL(0x0259,26,), SMALL(0x02A3,'d','z'), 
SMALL(0x02A6,'t','s'), SMALL(0x02AA,'l','s'), SMALL(0x02AB,'l','z'), SMALL(0x02B9,'\'',), 
SMALL(0x02BA,'"',), SMALL(0x02BC,'\'',), SMALL(0x02C2,'<',), SMALL(0x02C3,'>',), SMALL(0x02C6,'^',), 
SMALL(0x02C8,'\'',), SMALL(0x02C9,0xFF,), SMALL(0x02CA,0xBA,), SMALL(0x02CB,0x60,), SMALL(0x02CD,'_',), 
SMALL(0x02D0,':',), SMALL(0x02D1,0xFA,), SMALL(0x02D6,0x2B,), SMALL(0x02D7,'-',), SMALL(0x02DA,0xF8,), 
SMALL(0x02DC,'~',), SMALL(0x02EE,'"',), SMALL(0x034F,0,), SMALL(0x0374,0xBA,), SMALL(0x037E,';',), 
SMALL(0x0391,'A',), SMALL(0x0392,'B',), SMALL(0x0393,0xE2,), SMALL(0x0394,127,), SMALL(0x0395,'E',), 
SMALL(0x0396,'Z',), SMALL(0x0397,'H',), SMALL(0x0398,0xE9,), SMALL(0x0399,'I',), SMALL(0x039A,'K',), 
SMALL(0x039C,'M',), SMALL(0x039D,'N',), SMALL(0x039E,0xF0,), SMALL(0x039F,'O',), SMALL(0x03A0,0xEF,), 
SMALL(0x03A1,'P',), SMALL(0x03A3,0xE4,), SMALL(0x03A4,'T',), SMALL(0x03A5,'Y',), SMALL(0x03A6,0xE8,), 
SMALL(0x03A7,'X',), SMALL(0x03A9,0xEA,), SMALL(0x03B1,0xE0,), SMALL(0x03B2,0xE1,), SMALL(0x03B4,0xEB,), 
SMALL(0x03B5,0xEE,), SMALL(0x03BC,0xE6,), SMALL(0x03BD,'v',), SMALL(0x03BF,'o',), SMALL(0x03C0,0xE3,), 
SMALL(0x03C1,'p',), SMALL(0x03C2,'s',), SMALL(0x03C3,0xE5,), SMALL(0x03C4,0xE7,), SMALL(0x03C6,0xED,), 
SMALL(0x03D5,0xED,), SMALL(0x03DC,'F',), SMALL(0x03F2,'c',), SMALL(0x03F3,'j',), SMALL(0x03F4,0xE9,), 
SMALL(0x03F5,0xEE,), SMALL(0x05BE,0xFF,), SMALL(0x05C0,'|',), SMALL(0x05C3,':',), SMALL(0x05D0,0xC2,), 
SMALL(0x05D1,0xC3,), SMALL(0x05D2,0xC4,), SMALL(0x05D3,0xC5,), SMALL(0x05D4,0xC6,), SMALL(0x05D5,0xC7,), 
SMALL(0x05D6,0xC8,), SMALL(0x05D7,0xC9,), SMALL(0x05D8,0xCA,), SMALL(0x05D9,0xCB,), SMALL(0x05DA,0xD9,), 
SMALL(0x05DB,0xCC,), SMALL(0x05DC,0xCD,), SMALL(0x05DD,0xDA,), SMALL(0x05DE,0xCE,), SMALL(0x05DF,0xD8,), 
SMALL(0x05E0,0xCF,), SMALL(0x05E1,0xD0,), SMALL(0x05E2,0xD1,), SMALL(0x05E3,0xDB,), SMALL(0x05E4,0xD2,), 
SMALL(0x05E5,0xDC,), SMALL(0x05E6,0xD3,), SMALL(0x05E7,0xD4,), SMALL(0x05E8,0xD5,), SMALL(0x05E9,0xD6,), 
SMALL(0x05EA,0xD7,), SMALL(0x05F0,0xC7,0xC7), SMALL(0x05F1,0xCB,0xC7), SMALL(0x05F2,0xCB,0xCB), 
SMALL(0x05F3,'\'',), SMALL(0x05F4,'"',), SMALL(0x0660,'0',), SMALL(0x0661,'1',), SMALL(0x0662,'2',), 
SMALL(0x0663,'3',), SMALL(0x0664,'4',), SMALL(0x0665,'5',), SMALL(0x0666,'6',), SMALL(0x0667,'7',), 
SMALL(0x0668,'8',), SMALL(0x0669,'9',), SMALL(0x066A,'%',), SMALL(0x066B,',',), SMALL(0x066C,'\'',), 
SMALL(0x066D,'*',), SMALL(0x070F,0,), MULTI(0x09F3), MULTI(0x0E3F), 
SMALL(0x10FB,':',0xFA), MULTI(0x17DB), SMALL(0x180B,0,), SMALL(0x180C,0,), SMALL(0x180D,0,), 
SMALL(0x180E,0,), 
SMALL(0x2000,' ',), SMALL(0x2001,' ',' '), SMALL(0x2002,' ',), SMALL(0x2003,' ',' '), SMALL(0x2004,' ',), 
SMALL(0x2005,' ',), SMALL(0x2006,' ',), SMALL(0x2007,' ',), SMALL(0x2008,' ',), SMALL(0x2009,' ',), SMALL(0x200A,0,), 
SMALL(0x200B,0,), SMALL(0x200C,0,), SMALL(0x200D,0,), SMALL(0x2010,'-',), SMALL(0x2011,'-',), SMALL(0x2012,'-',), 
SMALL(0x2013,'-',), SMALL(0x2014,'-','-'), SMALL(0x2015,'-','-'), SMALL(0x2016,'|','|'), 
SMALL(0x2018,'\'',), SMALL(0x2019,'\'',), SMALL(0x201A,'\'',), SMALL(0x201B,'\'',), SMALL(0x201C,'"',), 
SMALL(0x201D,'"',), SMALL(0x201E,'"',), SMALL(0x201F,'"',), SMALL(0x2020,0xBB,), SMALL(0x2021,0xBB,), 
SMALL(0x2022,0xF9,), SMALL(0x2023,0xF9,), SMALL(0x2024,'.',), SMALL(0x2025,'.','.'), 
MULTI(0x2026), SMALL(0x2027,0xFA,), SMALL(0x202F,' ',), 
MULTI(0x2030), SMALL(0x2032,'\'',), SMALL(0x2333,'"',), SMALL(0x2039,'<',), 
SMALL(0x203A,'>',), SMALL(0x203C,'!','!'), SMALL(0x203E,0xFF,), SMALL(0x2043,'-',), SMALL(0x2044,'/',), 
SMALL(0x2047,'?','?'), SMALL(0x204A,'7',), SMALL(0x204C,0xF9,), SMALL(0x204D,0xF9,), 
SMALL(0x204E,'*',), SMALL(0x2052,'%',), SMALL(0x2057,'"','"'), 
SMALL(0x205F,' ',), SMALL(0x2060,0,), SMALL(0x2061,0,), SMALL(0x2062,' ',), 
SMALL(0x2063,' ',), SMALL(0x207F,0xFC,), SMALL(0x20A4,0x9C,), MULTI(0x20A7), 
SMALL(0x20A8,'R','s'), MULTI(0x20AA), SMALL(0x20AC,0xEE,), 
SMALL(0x20B0,'P','f'), MULTI(0x2100), MULTI(0x2101), 
SMALL(0x2103,0xF8,'C'), MULTI(0x2105), MULTI(0x2106), SMALL(0x2109,0xF8,'F'), 
SMALL(0x2113,'l',), SMALL(0x2114,'l','b'), SMALL(0x2116,'N',0xA7), SMALL(0x2122,0xBF,), 
SMALL(0x2126,0xEA,), SMALL(0x2127,'S',), SMALL(0x212A,'K',), SMALL(0x212B,0x8F,), SMALL(0x212E,'e',), 
SMALL(0x2133,'M',), SMALL(0x2135,0xC2,), SMALL(0x2136,0xC3,), SMALL(0x2137,0xC4,), SMALL(0x2138,0xC5,), 
SMALL(0x2139,'i',), SMALL(0x2180,0xDF,), SMALL(0x2190,4,), SMALL(0x2191,1,), SMALL(0x2192,3,), SMALL(0x2193,2,), 
SMALL(0x21E6,4,), SMALL(0x21E7,1,), SMALL(0x21E8,3,), SMALL(0x21E9,2,), SMALL(0x2205,0xED,), SMALL(0x2206,127,), 
SMALL(0x2208,0xee,), SMALL(0x220A,0xee,), SMALL(0x2211,0xe4,), SMALL(0x2212,'-',), SMALL(0x2215,'/',), 
SMALL(0x2216,'\\',), SMALL(0x2217,'*',), SMALL(0x2219,0xF9,), SMALL(0x221A,0xFB,), SMALL(0x221D,0xE0,), 
SMALL(0x221E,0xDF,), SMALL(0x2223,'|',), SMALL(0x2225,'|','|'), SMALL(0x2227,0xDE,), SMALL(0x2229,0xEF,), 
SMALL(0x222A,'U',), SMALL(0x222E,0xEC,), SMALL(0x2236,':',), SMALL(0x223C,'~',), SMALL(0x223F,'~',), 
SMALL(0x2248,0xF7,), SMALL(0x2261,0xF0,), SMALL(0x2264,0xF3,), SMALL(0x2265,0xF2,), 
MULTI(0x2295), MULTI(0x2297), 
MULTI(0x2299), SMALL(0x22BA,'T',), SMALL(0x22C0,0xDE,), SMALL(0x22C2,0xEF,), 
SMALL(0x22C3,'U',), SMALL(0x22C5,0xFA,), MULTI(0x22EF), SMALL(0x22FF,'E',), 
SMALL(0x2310,0xAA,), SMALL(0x2314,8,), SMALL(0x231A,9,), SMALL(0x2320,0xF4,), SMALL(0x2321,0xF5,), SMALL(0x2329,'<',), 
SMALL(0x232A,'>',), SMALL(0x23B7,0xFB,), SMALL(0x240C,12,), SMALL(0x240D,13,), SMALL(0x241B,27,), 
SMALL(0x266A,11,), MULTI(0x2680), MULTI(0x2681), 
MULTI(0x2682), MULTI(0x2683), 
MULTI(0x2684), MULTI(0x2685), 
SMALL(0x2713,8,), SMALL(0x2714,8,), SMALL(0x27E8,'<',), SMALL(0x27E9,'>',), 
SMALL(0x29F2,0xE8,), SMALL(0x2A0D,0x9F,), SMALL(0x2A2F,'x',), 
SMALL(0x3000,' ',' '), 
SMALL(0x301D,'"',), SMALL(0x301E,'"',), SMALL(0x301F,'"',), 
SMALL(0xF8F0,'j',), SMALL(0xFB00,'f','f'), SMALL(0xFB01,'f','i'), 
SMALL(0xFB02,'f','l'), MULTI(0xFB03), MULTI(0xFB04), 
SMALL(0xFB05,0xF4,'t'), SMALL(0xFB06,'s','t'), SMALL(0xFB20,0xD1,), SMALL(0xFB21,0xC2,), 
SMALL(0xFB22,0xC5,), SMALL(0xFB23,0xC6,), SMALL(0xFB24,0xCC,), SMALL(0xFB25,0xCD,), SMALL(0xFB26,0xDA,), 
SMALL(0xFB27,0xD5,), SMALL(0xFB28,0xD7,), SMALL(0xFB29,'+',), SMALL(0xFB4F,0xC2,0xCD), 
SMALL(0xFD3E,'(',), SMALL(0xFD3F,')',), SMALL(0xFE00,0,), SMALL(0xFE01,0,), 
SMALL(0xFE02,0,), SMALL(0xFE03,0,), SMALL(0xFE04,0,), SMALL(0xFE05,0,), 
SMALL(0xFE06,0,), SMALL(0xFE07,0,), SMALL(0xFE08,0,), SMALL(0xFE09,0,), 
SMALL(0xFE0A,0,), SMALL(0xFE0B,0,), SMALL(0xFE0C,0,), SMALL(0xFE0D,0,), 
SMALL(0xFE0E,0,), SMALL(0xFE0F,0,), SMALL(0xFEFF,0,), 
SMALL(/*This_MUST_be_the_last_entry!*/0xFFFD,6,) };
#undef SMALL
#undef MULTI
