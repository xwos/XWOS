/**
 * @file
 * @brief 玄武OS通用库：字符类型
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <xwos/lib/ctype.h>

/**
 * @brief 字符类型表
 */
__xwlib_rodata const unsigned char xwlib_ctype[] = {
        XWLIB_CTYPE_FLAG_CTRL,                                  /* 0: NUL */
        XWLIB_CTYPE_FLAG_CTRL,                                  /* 1: SOH */
        XWLIB_CTYPE_FLAG_CTRL,                                  /* 2: STX */
        XWLIB_CTYPE_FLAG_CTRL,                                  /* 3: ETX */
        XWLIB_CTYPE_FLAG_CTRL,                                  /* 4: EOT */
        XWLIB_CTYPE_FLAG_CTRL,                                  /* 5: ENQ */
        XWLIB_CTYPE_FLAG_CTRL,                                  /* 6: ACK */
        XWLIB_CTYPE_FLAG_CTRL,                                  /* 7: BEL '\a' */
        XWLIB_CTYPE_FLAG_CTRL,                                  /* 8: BS '\b' */
        XWLIB_CTYPE_FLAG_CTRL | XWLIB_CTYPE_FLAG_WSPC,          /* 9: TAB '\t' */
        XWLIB_CTYPE_FLAG_CTRL | XWLIB_CTYPE_FLAG_WSPC,          /* 10: LF '\n' */
        XWLIB_CTYPE_FLAG_CTRL | XWLIB_CTYPE_FLAG_WSPC,          /* 11: VT '\v' */
        XWLIB_CTYPE_FLAG_CTRL | XWLIB_CTYPE_FLAG_WSPC,          /* 12: FF '\f' */
        XWLIB_CTYPE_FLAG_CTRL | XWLIB_CTYPE_FLAG_WSPC,          /* 13: CR '\r' */
        XWLIB_CTYPE_FLAG_CTRL,                                  /* 14: SO */
        XWLIB_CTYPE_FLAG_CTRL,                                  /* 15: SI */
        XWLIB_CTYPE_FLAG_CTRL,                                  /* 16: DLE */
        XWLIB_CTYPE_FLAG_CTRL,                                  /* 17: DC1 */
        XWLIB_CTYPE_FLAG_CTRL,                                  /* 18: DC2 */
        XWLIB_CTYPE_FLAG_CTRL,                                  /* 19: DC3 */
        XWLIB_CTYPE_FLAG_CTRL,                                  /* 20: DC4 */
        XWLIB_CTYPE_FLAG_CTRL,                                  /* 21: NAK */
        XWLIB_CTYPE_FLAG_CTRL,                                  /* 22: SYN */
        XWLIB_CTYPE_FLAG_CTRL,                                  /* 23: ETB */
        XWLIB_CTYPE_FLAG_CTRL,                                  /* 24: CAN */
        XWLIB_CTYPE_FLAG_CTRL,                                  /* 25: EM */
        XWLIB_CTYPE_FLAG_CTRL,                                  /* 26: SUB */
        XWLIB_CTYPE_FLAG_CTRL,                                  /* 27: ESC '\e '*/
        XWLIB_CTYPE_FLAG_CTRL,                                  /* 28: FS */
        XWLIB_CTYPE_FLAG_CTRL,                                  /* 29: GS */
        XWLIB_CTYPE_FLAG_CTRL,                                  /* 30: RS */
        XWLIB_CTYPE_FLAG_CTRL,                                  /* 31: UF */
        XWLIB_CTYPE_FLAG_WSPC | XWLIB_CTYPE_FLAG_SPC,           /* 32: ' ' */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 33: ! */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 34: " */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 35: # */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 36: $ */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 37: % */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 38: & */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 39: ' */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 40: ( */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 41: ) */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 42: * */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 43: + */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 44: , */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 45: - */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 46: . */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 47: / */
        XWLIB_CTYPE_FLAG_DIGIT,                                 /* 48: 0 */
        XWLIB_CTYPE_FLAG_DIGIT,                                 /* 49: 1 */
        XWLIB_CTYPE_FLAG_DIGIT,                                 /* 50: 2 */
        XWLIB_CTYPE_FLAG_DIGIT,                                 /* 51: 3 */
        XWLIB_CTYPE_FLAG_DIGIT,                                 /* 52: 4 */
        XWLIB_CTYPE_FLAG_DIGIT,                                 /* 53: 5 */
        XWLIB_CTYPE_FLAG_DIGIT,                                 /* 54: 6 */
        XWLIB_CTYPE_FLAG_DIGIT,                                 /* 55: 7 */
        XWLIB_CTYPE_FLAG_DIGIT,                                 /* 56: 8 */
        XWLIB_CTYPE_FLAG_DIGIT,                                 /* 57: 9 */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 58: : */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 59: ; */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 60: < */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 61: = */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 62: > */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 63: ? */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 64: @ */
        XWLIB_CTYPE_FLAG_UPPER | XWLIB_CTYPE_FLAG_HEX,          /* 65: A */
        XWLIB_CTYPE_FLAG_UPPER | XWLIB_CTYPE_FLAG_HEX,          /* 66: B */
        XWLIB_CTYPE_FLAG_UPPER | XWLIB_CTYPE_FLAG_HEX,          /* 67: C */
        XWLIB_CTYPE_FLAG_UPPER | XWLIB_CTYPE_FLAG_HEX,          /* 68: D */
        XWLIB_CTYPE_FLAG_UPPER | XWLIB_CTYPE_FLAG_HEX,          /* 69: E */
        XWLIB_CTYPE_FLAG_UPPER | XWLIB_CTYPE_FLAG_HEX,          /* 70: F */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 71: G */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 72: H */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 73: I */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 74: J */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 75: K */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 76: L */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 77: M */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 78: N */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 79: O */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 80: P */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 81: Q */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 82: R */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 83: S */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 84: T */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 85: U */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 86: V */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 87: W */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 88: X */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 89: Y */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 90: Z */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 91: [ */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 92: \ */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 93: ] */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 94: ^ */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 95: _ */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 96: ` */
        XWLIB_CTYPE_FLAG_LOWER | XWLIB_CTYPE_FLAG_HEX,          /* 97: a */
        XWLIB_CTYPE_FLAG_LOWER | XWLIB_CTYPE_FLAG_HEX,          /* 98: b */
        XWLIB_CTYPE_FLAG_LOWER | XWLIB_CTYPE_FLAG_HEX,          /* 99: c */
        XWLIB_CTYPE_FLAG_LOWER | XWLIB_CTYPE_FLAG_HEX,          /* 100: d */
        XWLIB_CTYPE_FLAG_LOWER | XWLIB_CTYPE_FLAG_HEX,          /* 101: e */
        XWLIB_CTYPE_FLAG_LOWER | XWLIB_CTYPE_FLAG_HEX,          /* 102: f */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 103: g */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 104: h */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 105: i */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 106: j */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 107: k */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 108: l */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 109: m */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 110: n */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 111: o */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 112: p */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 113: q */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 114: r */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 115: s */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 116: t */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 117: u */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 118: v */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 119: w */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 120: x */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 121: y */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 122: z */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 123: { */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 124: | */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 125: } */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 126: ~ */
        XWLIB_CTYPE_FLAG_CTRL,                                  /* 127: */
        0,                                                      /* 128: */
        0,                                                      /* 129: */
        0,                                                      /* 130: */
        0,                                                      /* 131: */
        0,                                                      /* 132: */
        0,                                                      /* 133: */
        0,                                                      /* 134: */
        0,                                                      /* 135: */
        0,                                                      /* 136: */
        0,                                                      /* 137: */
        0,                                                      /* 138: */
        0,                                                      /* 139: */
        0,                                                      /* 140: */
        0,                                                      /* 141: */
        0,                                                      /* 142: */
        0,                                                      /* 143: */
        0,                                                      /* 144: */
        0,                                                      /* 145: */
        0,                                                      /* 146: */
        0,                                                      /* 147: */
        0,                                                      /* 148: */
        0,                                                      /* 149: */
        0,                                                      /* 150: */
        0,                                                      /* 151: */
        0,                                                      /* 152: */
        0,                                                      /* 153: */
        0,                                                      /* 154: */
        0,                                                      /* 155: */
        0,                                                      /* 156: */
        0,                                                      /* 157: */
        0,                                                      /* 158: */
        0,                                                      /* 159: */
        XWLIB_CTYPE_FLAG_WSPC | XWLIB_CTYPE_FLAG_SPC,           /* 160: */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 161: */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 162: */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 163: */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 164: */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 165: */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 166: */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 167: */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 168: */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 169: */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 170: */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 171: */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 172: */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 173: */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 174: */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 175: */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 176: */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 177: */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 178: */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 179: */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 180: */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 181: */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 182: */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 183: */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 184: */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 185: */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 186: */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 187: */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 188: */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 189: */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 190: */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 191: */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 192: */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 193: */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 194: */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 195: */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 196: */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 197: */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 198: */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 199: */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 200: */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 201: */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 202: */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 203: */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 204: */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 205: */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 206: */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 207: */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 208: */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 209: */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 210: */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 211: */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 212: */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 213: */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 214: */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 215: */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 216: */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 217: */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 218: */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 219: */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 220: */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 221: */
        XWLIB_CTYPE_FLAG_UPPER,                                 /* 222: */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 223: */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 224: */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 225: */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 226: */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 227: */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 228: */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 229: */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 230: */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 231: */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 232: */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 233: */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 234: */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 235: */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 236: */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 237: */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 238: */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 239: */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 240: */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 241: */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 242: */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 243: */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 244: */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 245: */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 246: */
        XWLIB_CTYPE_FLAG_PUNCT,                                 /* 247: */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 248: */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 249: */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 250: */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 251: */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 252: */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 253: */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 254: */
        XWLIB_CTYPE_FLAG_LOWER,                                 /* 255: */
};
