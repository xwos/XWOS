/**
 * @file
 * @brief XWOS通用库：CRC32
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/crc32.h>

#if defined(XWLIBCFG_CRC32_0X04C11DB7) && (1 == XWLIBCFG_CRC32_0X04C11DB7)
/**
 * @brief 多项式0x04C11DB7的查询表
 */
__xwlib_rodata const xwu32_t xwlib_crc32tbl_0x04c11db7[256] = {
        [0] = 0x0,
        [1] = 0x4C11DB7,
        [2] = 0x9823B6E,
        [3] = 0xD4326D9,
        [4] = 0x130476DC,
        [5] = 0x17C56B6B,
        [6] = 0x1A864DB2,
        [7] = 0x1E475005,
        [8] = 0x2608EDB8,
        [9] = 0x22C9F00F,
        [10] = 0x2F8AD6D6,
        [11] = 0x2B4BCB61,
        [12] = 0x350C9B64,
        [13] = 0x31CD86D3,
        [14] = 0x3C8EA00A,
        [15] = 0x384FBDBD,
        [16] = 0x4C11DB70,
        [17] = 0x48D0C6C7,
        [18] = 0x4593E01E,
        [19] = 0x4152FDA9,
        [20] = 0x5F15ADAC,
        [21] = 0x5BD4B01B,
        [22] = 0x569796C2,
        [23] = 0x52568B75,
        [24] = 0x6A1936C8,
        [25] = 0x6ED82B7F,
        [26] = 0x639B0DA6,
        [27] = 0x675A1011,
        [28] = 0x791D4014,
        [29] = 0x7DDC5DA3,
        [30] = 0x709F7B7A,
        [31] = 0x745E66CD,
        [32] = 0x9823B6E0,
        [33] = 0x9CE2AB57,
        [34] = 0x91A18D8E,
        [35] = 0x95609039,
        [36] = 0x8B27C03C,
        [37] = 0x8FE6DD8B,
        [38] = 0x82A5FB52,
        [39] = 0x8664E6E5,
        [40] = 0xBE2B5B58,
        [41] = 0xBAEA46EF,
        [42] = 0xB7A96036,
        [43] = 0xB3687D81,
        [44] = 0xAD2F2D84,
        [45] = 0xA9EE3033,
        [46] = 0xA4AD16EA,
        [47] = 0xA06C0B5D,
        [48] = 0xD4326D90,
        [49] = 0xD0F37027,
        [50] = 0xDDB056FE,
        [51] = 0xD9714B49,
        [52] = 0xC7361B4C,
        [53] = 0xC3F706FB,
        [54] = 0xCEB42022,
        [55] = 0xCA753D95,
        [56] = 0xF23A8028,
        [57] = 0xF6FB9D9F,
        [58] = 0xFBB8BB46,
        [59] = 0xFF79A6F1,
        [60] = 0xE13EF6F4,
        [61] = 0xE5FFEB43,
        [62] = 0xE8BCCD9A,
        [63] = 0xEC7DD02D,
        [64] = 0x34867077,
        [65] = 0x30476DC0,
        [66] = 0x3D044B19,
        [67] = 0x39C556AE,
        [68] = 0x278206AB,
        [69] = 0x23431B1C,
        [70] = 0x2E003DC5,
        [71] = 0x2AC12072,
        [72] = 0x128E9DCF,
        [73] = 0x164F8078,
        [74] = 0x1B0CA6A1,
        [75] = 0x1FCDBB16,
        [76] = 0x18AEB13,
        [77] = 0x54BF6A4,
        [78] = 0x808D07D,
        [79] = 0xCC9CDCA,
        [80] = 0x7897AB07,
        [81] = 0x7C56B6B0,
        [82] = 0x71159069,
        [83] = 0x75D48DDE,
        [84] = 0x6B93DDDB,
        [85] = 0x6F52C06C,
        [86] = 0x6211E6B5,
        [87] = 0x66D0FB02,
        [88] = 0x5E9F46BF,
        [89] = 0x5A5E5B08,
        [90] = 0x571D7DD1,
        [91] = 0x53DC6066,
        [92] = 0x4D9B3063,
        [93] = 0x495A2DD4,
        [94] = 0x44190B0D,
        [95] = 0x40D816BA,
        [96] = 0xACA5C697,
        [97] = 0xA864DB20,
        [98] = 0xA527FDF9,
        [99] = 0xA1E6E04E,
        [100] = 0xBFA1B04B,
        [101] = 0xBB60ADFC,
        [102] = 0xB6238B25,
        [103] = 0xB2E29692,
        [104] = 0x8AAD2B2F,
        [105] = 0x8E6C3698,
        [106] = 0x832F1041,
        [107] = 0x87EE0DF6,
        [108] = 0x99A95DF3,
        [109] = 0x9D684044,
        [110] = 0x902B669D,
        [111] = 0x94EA7B2A,
        [112] = 0xE0B41DE7,
        [113] = 0xE4750050,
        [114] = 0xE9362689,
        [115] = 0xEDF73B3E,
        [116] = 0xF3B06B3B,
        [117] = 0xF771768C,
        [118] = 0xFA325055,
        [119] = 0xFEF34DE2,
        [120] = 0xC6BCF05F,
        [121] = 0xC27DEDE8,
        [122] = 0xCF3ECB31,
        [123] = 0xCBFFD686,
        [124] = 0xD5B88683,
        [125] = 0xD1799B34,
        [126] = 0xDC3ABDED,
        [127] = 0xD8FBA05A,
        [128] = 0x690CE0EE,
        [129] = 0x6DCDFD59,
        [130] = 0x608EDB80,
        [131] = 0x644FC637,
        [132] = 0x7A089632,
        [133] = 0x7EC98B85,
        [134] = 0x738AAD5C,
        [135] = 0x774BB0EB,
        [136] = 0x4F040D56,
        [137] = 0x4BC510E1,
        [138] = 0x46863638,
        [139] = 0x42472B8F,
        [140] = 0x5C007B8A,
        [141] = 0x58C1663D,
        [142] = 0x558240E4,
        [143] = 0x51435D53,
        [144] = 0x251D3B9E,
        [145] = 0x21DC2629,
        [146] = 0x2C9F00F0,
        [147] = 0x285E1D47,
        [148] = 0x36194D42,
        [149] = 0x32D850F5,
        [150] = 0x3F9B762C,
        [151] = 0x3B5A6B9B,
        [152] = 0x315D626,
        [153] = 0x7D4CB91,
        [154] = 0xA97ED48,
        [155] = 0xE56F0FF,
        [156] = 0x1011A0FA,
        [157] = 0x14D0BD4D,
        [158] = 0x19939B94,
        [159] = 0x1D528623,
        [160] = 0xF12F560E,
        [161] = 0xF5EE4BB9,
        [162] = 0xF8AD6D60,
        [163] = 0xFC6C70D7,
        [164] = 0xE22B20D2,
        [165] = 0xE6EA3D65,
        [166] = 0xEBA91BBC,
        [167] = 0xEF68060B,
        [168] = 0xD727BBB6,
        [169] = 0xD3E6A601,
        [170] = 0xDEA580D8,
        [171] = 0xDA649D6F,
        [172] = 0xC423CD6A,
        [173] = 0xC0E2D0DD,
        [174] = 0xCDA1F604,
        [175] = 0xC960EBB3,
        [176] = 0xBD3E8D7E,
        [177] = 0xB9FF90C9,
        [178] = 0xB4BCB610,
        [179] = 0xB07DABA7,
        [180] = 0xAE3AFBA2,
        [181] = 0xAAFBE615,
        [182] = 0xA7B8C0CC,
        [183] = 0xA379DD7B,
        [184] = 0x9B3660C6,
        [185] = 0x9FF77D71,
        [186] = 0x92B45BA8,
        [187] = 0x9675461F,
        [188] = 0x8832161A,
        [189] = 0x8CF30BAD,
        [190] = 0x81B02D74,
        [191] = 0x857130C3,
        [192] = 0x5D8A9099,
        [193] = 0x594B8D2E,
        [194] = 0x5408ABF7,
        [195] = 0x50C9B640,
        [196] = 0x4E8EE645,
        [197] = 0x4A4FFBF2,
        [198] = 0x470CDD2B,
        [199] = 0x43CDC09C,
        [200] = 0x7B827D21,
        [201] = 0x7F436096,
        [202] = 0x7200464F,
        [203] = 0x76C15BF8,
        [204] = 0x68860BFD,
        [205] = 0x6C47164A,
        [206] = 0x61043093,
        [207] = 0x65C52D24,
        [208] = 0x119B4BE9,
        [209] = 0x155A565E,
        [210] = 0x18197087,
        [211] = 0x1CD86D30,
        [212] = 0x29F3D35,
        [213] = 0x65E2082,
        [214] = 0xB1D065B,
        [215] = 0xFDC1BEC,
        [216] = 0x3793A651,
        [217] = 0x3352BBE6,
        [218] = 0x3E119D3F,
        [219] = 0x3AD08088,
        [220] = 0x2497D08D,
        [221] = 0x2056CD3A,
        [222] = 0x2D15EBE3,
        [223] = 0x29D4F654,
        [224] = 0xC5A92679,
        [225] = 0xC1683BCE,
        [226] = 0xCC2B1D17,
        [227] = 0xC8EA00A0,
        [228] = 0xD6AD50A5,
        [229] = 0xD26C4D12,
        [230] = 0xDF2F6BCB,
        [231] = 0xDBEE767C,
        [232] = 0xE3A1CBC1,
        [233] = 0xE760D676,
        [234] = 0xEA23F0AF,
        [235] = 0xEEE2ED18,
        [236] = 0xF0A5BD1D,
        [237] = 0xF464A0AA,
        [238] = 0xF9278673,
        [239] = 0xFDE69BC4,
        [240] = 0x89B8FD09,
        [241] = 0x8D79E0BE,
        [242] = 0x803AC667,
        [243] = 0x84FBDBD0,
        [244] = 0x9ABC8BD5,
        [245] = 0x9E7D9662,
        [246] = 0x933EB0BB,
        [247] = 0x97FFAD0C,
        [248] = 0xAFB010B1,
        [249] = 0xAB710D06,
        [250] = 0xA6322BDF,
        [251] = 0xA2F33668,
        [252] = 0xBCB4666D,
        [253] = 0xB8757BDA,
        [254] = 0xB5365D03,
        [255] = 0xB1F740B4,
};
#endif

#if defined(XWLIBCFG_CRC32_0XEDB88320) && (1 == XWLIBCFG_CRC32_0XEDB88320)
/**
 * @brief 多项式0XEDB88320的查询表
 */
__xwlib_rodata const xwu32_t xwlib_crc32tbl_0xedb88320[256] = {
        [0] = 0x0,
        [1] = 0x77073096,
        [2] = 0xEE0E612C,
        [3] = 0x990951BA,
        [4] = 0x76DC419,
        [5] = 0x706AF48F,
        [6] = 0xE963A535,
        [7] = 0x9E6495A3,
        [8] = 0xEDB8832,
        [9] = 0x79DCB8A4,
        [10] = 0xE0D5E91E,
        [11] = 0x97D2D988,
        [12] = 0x9B64C2B,
        [13] = 0x7EB17CBD,
        [14] = 0xE7B82D07,
        [15] = 0x90BF1D91,
        [16] = 0x1DB71064,
        [17] = 0x6AB020F2,
        [18] = 0xF3B97148,
        [19] = 0x84BE41DE,
        [20] = 0x1ADAD47D,
        [21] = 0x6DDDE4EB,
        [22] = 0xF4D4B551,
        [23] = 0x83D385C7,
        [24] = 0x136C9856,
        [25] = 0x646BA8C0,
        [26] = 0xFD62F97A,
        [27] = 0x8A65C9EC,
        [28] = 0x14015C4F,
        [29] = 0x63066CD9,
        [30] = 0xFA0F3D63,
        [31] = 0x8D080DF5,
        [32] = 0x3B6E20C8,
        [33] = 0x4C69105E,
        [34] = 0xD56041E4,
        [35] = 0xA2677172,
        [36] = 0x3C03E4D1,
        [37] = 0x4B04D447,
        [38] = 0xD20D85FD,
        [39] = 0xA50AB56B,
        [40] = 0x35B5A8FA,
        [41] = 0x42B2986C,
        [42] = 0xDBBBC9D6,
        [43] = 0xACBCF940,
        [44] = 0x32D86CE3,
        [45] = 0x45DF5C75,
        [46] = 0xDCD60DCF,
        [47] = 0xABD13D59,
        [48] = 0x26D930AC,
        [49] = 0x51DE003A,
        [50] = 0xC8D75180,
        [51] = 0xBFD06116,
        [52] = 0x21B4F4B5,
        [53] = 0x56B3C423,
        [54] = 0xCFBA9599,
        [55] = 0xB8BDA50F,
        [56] = 0x2802B89E,
        [57] = 0x5F058808,
        [58] = 0xC60CD9B2,
        [59] = 0xB10BE924,
        [60] = 0x2F6F7C87,
        [61] = 0x58684C11,
        [62] = 0xC1611DAB,
        [63] = 0xB6662D3D,
        [64] = 0x76DC4190,
        [65] = 0x1DB7106,
        [66] = 0x98D220BC,
        [67] = 0xEFD5102A,
        [68] = 0x71B18589,
        [69] = 0x6B6B51F,
        [70] = 0x9FBFE4A5,
        [71] = 0xE8B8D433,
        [72] = 0x7807C9A2,
        [73] = 0xF00F934,
        [74] = 0x9609A88E,
        [75] = 0xE10E9818,
        [76] = 0x7F6A0DBB,
        [77] = 0x86D3D2D,
        [78] = 0x91646C97,
        [79] = 0xE6635C01,
        [80] = 0x6B6B51F4,
        [81] = 0x1C6C6162,
        [82] = 0x856530D8,
        [83] = 0xF262004E,
        [84] = 0x6C0695ED,
        [85] = 0x1B01A57B,
        [86] = 0x8208F4C1,
        [87] = 0xF50FC457,
        [88] = 0x65B0D9C6,
        [89] = 0x12B7E950,
        [90] = 0x8BBEB8EA,
        [91] = 0xFCB9887C,
        [92] = 0x62DD1DDF,
        [93] = 0x15DA2D49,
        [94] = 0x8CD37CF3,
        [95] = 0xFBD44C65,
        [96] = 0x4DB26158,
        [97] = 0x3AB551CE,
        [98] = 0xA3BC0074,
        [99] = 0xD4BB30E2,
        [100] = 0x4ADFA541,
        [101] = 0x3DD895D7,
        [102] = 0xA4D1C46D,
        [103] = 0xD3D6F4FB,
        [104] = 0x4369E96A,
        [105] = 0x346ED9FC,
        [106] = 0xAD678846,
        [107] = 0xDA60B8D0,
        [108] = 0x44042D73,
        [109] = 0x33031DE5,
        [110] = 0xAA0A4C5F,
        [111] = 0xDD0D7CC9,
        [112] = 0x5005713C,
        [113] = 0x270241AA,
        [114] = 0xBE0B1010,
        [115] = 0xC90C2086,
        [116] = 0x5768B525,
        [117] = 0x206F85B3,
        [118] = 0xB966D409,
        [119] = 0xCE61E49F,
        [120] = 0x5EDEF90E,
        [121] = 0x29D9C998,
        [122] = 0xB0D09822,
        [123] = 0xC7D7A8B4,
        [124] = 0x59B33D17,
        [125] = 0x2EB40D81,
        [126] = 0xB7BD5C3B,
        [127] = 0xC0BA6CAD,
        [128] = 0xEDB88320,
        [129] = 0x9ABFB3B6,
        [130] = 0x3B6E20C,
        [131] = 0x74B1D29A,
        [132] = 0xEAD54739,
        [133] = 0x9DD277AF,
        [134] = 0x4DB2615,
        [135] = 0x73DC1683,
        [136] = 0xE3630B12,
        [137] = 0x94643B84,
        [138] = 0xD6D6A3E,
        [139] = 0x7A6A5AA8,
        [140] = 0xE40ECF0B,
        [141] = 0x9309FF9D,
        [142] = 0xA00AE27,
        [143] = 0x7D079EB1,
        [144] = 0xF00F9344,
        [145] = 0x8708A3D2,
        [146] = 0x1E01F268,
        [147] = 0x6906C2FE,
        [148] = 0xF762575D,
        [149] = 0x806567CB,
        [150] = 0x196C3671,
        [151] = 0x6E6B06E7,
        [152] = 0xFED41B76,
        [153] = 0x89D32BE0,
        [154] = 0x10DA7A5A,
        [155] = 0x67DD4ACC,
        [156] = 0xF9B9DF6F,
        [157] = 0x8EBEEFF9,
        [158] = 0x17B7BE43,
        [159] = 0x60B08ED5,
        [160] = 0xD6D6A3E8,
        [161] = 0xA1D1937E,
        [162] = 0x38D8C2C4,
        [163] = 0x4FDFF252,
        [164] = 0xD1BB67F1,
        [165] = 0xA6BC5767,
        [166] = 0x3FB506DD,
        [167] = 0x48B2364B,
        [168] = 0xD80D2BDA,
        [169] = 0xAF0A1B4C,
        [170] = 0x36034AF6,
        [171] = 0x41047A60,
        [172] = 0xDF60EFC3,
        [173] = 0xA867DF55,
        [174] = 0x316E8EEF,
        [175] = 0x4669BE79,
        [176] = 0xCB61B38C,
        [177] = 0xBC66831A,
        [178] = 0x256FD2A0,
        [179] = 0x5268E236,
        [180] = 0xCC0C7795,
        [181] = 0xBB0B4703,
        [182] = 0x220216B9,
        [183] = 0x5505262F,
        [184] = 0xC5BA3BBE,
        [185] = 0xB2BD0B28,
        [186] = 0x2BB45A92,
        [187] = 0x5CB36A04,
        [188] = 0xC2D7FFA7,
        [189] = 0xB5D0CF31,
        [190] = 0x2CD99E8B,
        [191] = 0x5BDEAE1D,
        [192] = 0x9B64C2B0,
        [193] = 0xEC63F226,
        [194] = 0x756AA39C,
        [195] = 0x26D930A,
        [196] = 0x9C0906A9,
        [197] = 0xEB0E363F,
        [198] = 0x72076785,
        [199] = 0x5005713,
        [200] = 0x95BF4A82,
        [201] = 0xE2B87A14,
        [202] = 0x7BB12BAE,
        [203] = 0xCB61B38,
        [204] = 0x92D28E9B,
        [205] = 0xE5D5BE0D,
        [206] = 0x7CDCEFB7,
        [207] = 0xBDBDF21,
        [208] = 0x86D3D2D4,
        [209] = 0xF1D4E242,
        [210] = 0x68DDB3F8,
        [211] = 0x1FDA836E,
        [212] = 0x81BE16CD,
        [213] = 0xF6B9265B,
        [214] = 0x6FB077E1,
        [215] = 0x18B74777,
        [216] = 0x88085AE6,
        [217] = 0xFF0F6A70,
        [218] = 0x66063BCA,
        [219] = 0x11010B5C,
        [220] = 0x8F659EFF,
        [221] = 0xF862AE69,
        [222] = 0x616BFFD3,
        [223] = 0x166CCF45,
        [224] = 0xA00AE278,
        [225] = 0xD70DD2EE,
        [226] = 0x4E048354,
        [227] = 0x3903B3C2,
        [228] = 0xA7672661,
        [229] = 0xD06016F7,
        [230] = 0x4969474D,
        [231] = 0x3E6E77DB,
        [232] = 0xAED16A4A,
        [233] = 0xD9D65ADC,
        [234] = 0x40DF0B66,
        [235] = 0x37D83BF0,
        [236] = 0xA9BCAE53,
        [237] = 0xDEBB9EC5,
        [238] = 0x47B2CF7F,
        [239] = 0x30B5FFE9,
        [240] = 0xBDBDF21C,
        [241] = 0xCABAC28A,
        [242] = 0x53B39330,
        [243] = 0x24B4A3A6,
        [244] = 0xBAD03605,
        [245] = 0xCDD70693,
        [246] = 0x54DE5729,
        [247] = 0x23D967BF,
        [248] = 0xB3667A2E,
        [249] = 0xC4614AB8,
        [250] = 0x5D681B02,
        [251] = 0x2A6F2B94,
        [252] = 0xB40BBE37,
        [253] = 0xC30C8EA1,
        [254] = 0x5A05DF1B,
        [255] = 0x2D02EF8D,
};
#endif

#if defined(SOCCFG_CRC32) && (1 == SOCCFG_CRC32)
extern
xwer_t soc_crc32_cal(xwu32_t * crc32,
                     bool refin, xwu32_t plynml, xwu32_t direction,
                     const xwu8_t stream[], xwsz_t * size);
#else
#  define soc_crc32_cal(crc32, refin, plynml, direction, stream, size) (-EOPNOTSUPP)
#endif

static __xwlib_code
void xwlib_crc32_swcal_ls(xwu32_t * crc32,
                          bool refin, const xwu32_t table[],
                          const xwu8_t stream[], xwsz_t * size);

static __xwlib_code
void xwlib_crc32_swcal_rs(xwu32_t * crc32,
                          bool refin, const xwu32_t table[],
                          const xwu8_t stream[], xwsz_t * size);

static __xwlib_code
xwer_t xwlib_crc32_swcal(xwu32_t * crc32,
                         bool refin, xwu32_t plynml, xwu32_t direction,
                         const xwu8_t stream[], xwsz_t * size);

/**
 * @brief 左移计算一段数据的CRC32校验值（直驱表法）
 * @param[in,out] crc32: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示初始值
 * + (O) 作为输出时，返回计算结果
 * @param[in] refin: 是否按位镜像翻转输入的每个字节(xwbop_rbit8)
 * @param[in] table: CRC32查询表
 * @param[in] stream: 指向数据的指针
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示数据长度
 * + (O) 作为输出时，返回剩余未计算的数据长度
 */
static __xwlib_code
void xwlib_crc32_swcal_ls(xwu32_t * crc32,
                          bool refin, const xwu32_t table[],
                          const xwu8_t stream[], xwsz_t * size)
{
        xwsz_t i;
        xwu8_t index, byte;
        xwu32_t remainder = *crc32;

        for (i = 0; i < *size; i++) {
                if (refin) {
                        byte = xwbop_rbit8(stream[i]);
                } else {
                        byte = stream[i];
                }
                index = (xwu8_t)((remainder >> 24) ^ byte);
                remainder = table[index] ^ (remainder << 8);
        }
        *crc32 = remainder;
        *size = 0;
}

/**
 * @brief 右移计算一段数据的CRC32校验值（直驱表法）
 * @param[in,out] crc32: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示初始值
 * + (O) 作为输出时，返回计算结果
 * @param[in] refin: 是否按位镜像翻转输入的每个字节(xwbop_rbit8)
 * @param[in] table: CRC32查询表
 * @param[in] stream: 指向数据的指针
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示数据长度
 * + (O) 作为输出时，返回剩余未计算的数据长度
 */
static __xwlib_code
void xwlib_crc32_swcal_rs(xwu32_t * crc32,
                          bool refin, const xwu32_t table[],
                          const xwu8_t stream[], xwsz_t * size)
{
        xwsz_t i;
        xwu8_t index, byte;
        xwu32_t remainder = *crc32;

        for (i = 0; i < *size; i++) {
                if (refin) {
                        byte = xwbop_rbit8(stream[i]);
                } else {
                        byte = stream[i];
                }
                index = (xwu8_t)((remainder & 0xFF) ^ byte);
                remainder = table[index] ^ (remainder >> 8);
        }
        *crc32 = remainder;
        *size = 0;
}

/**
 * @brief 软件方式计算一段数据的CRC32校验值（直驱表法）
 * @param[in,out] crc32: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示初始值
 * + (O) 作为输出时，返回计算结果
 * @param[in] refin: 是否按位镜像翻转输入的每个字节(xwbop_rbit8)
 * @param[in] plynml: 多项式
 * @param[in] direction: 数据移位的方向
 * @param[in] stream: 指向数据的指针
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示数据长度
 * + (O) 作为输出时，返回剩余未计算的数据长度
 * @return 错误码
 * @retval XWOK: 没有错误，计算结果有效
 * @retval -EOPNOTSUPP: 不支持的多项式
 */
static __xwlib_code
xwer_t xwlib_crc32_swcal(xwu32_t * crc32,
                         bool refin, xwu32_t plynml, xwu32_t direction,
                         const xwu8_t stream[], xwsz_t * size)
{
        xwer_t rc;

        switch (plynml) {
#if defined(XWLIBCFG_CRC32_0X04C11DB7) && (1 == XWLIBCFG_CRC32_0X04C11DB7)
        case 0x04C11DB7:
                if (XWLIB_CRC32_LEFT_SHIFT == direction) {
                        xwlib_crc32_swcal_ls(crc32, refin, xwlib_crc32tbl_0x04c11db7,
                                             stream, size);
                } else {
                        xwlib_crc32_swcal_rs(crc32, refin, xwlib_crc32tbl_0x04c11db7,
                                             stream, size);
                }
                rc = XWOK;
                break;
#endif

#if defined(XWLIBCFG_CRC32_0XEDB88320) && (1 == XWLIBCFG_CRC32_0XEDB88320)
        case 0xEDB88320:
                if (XWLIB_CRC32_RIGHT_SHIFT == direction) {
                        xwlib_crc32_swcal_rs(crc32, refin, xwlib_crc32tbl_0xedb88320,
                                             stream, size);
                } else {
                        xwlib_crc32_swcal_ls(crc32, refin, xwlib_crc32tbl_0xedb88320,
                                             stream, size);
                }
                rc = XWOK;
                break;
#endif
        default:
                XWOS_UNUSED(crc32);
                XWOS_UNUSED(refin);
                XWOS_UNUSED(plynml);
                XWOS_UNUSED(stream);
                XWOS_UNUSED(size);
                rc = -EOPNOTSUPP;
                break;
        }
        return rc;
}

/**
 * @brief 计算一段数据的CRC32校验值
 * @param[in,out] crc32: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示初始值
 * + (O) 作为输出时，返回计算结果，当计算失败时，值不会发生改变
 * @param[in] xorout: 与结果异或的值
 * @param[in] refin: 是否按位镜像翻转输入的每个字节(xwbop_rbit8)
 * @param[in] refout: 是否按位镜像翻转输出(xwbop_rbit32)
 * @param[in] plynml: 多项式
 * @param[in] direction: 数据移位的方向
 * @param[in] stream: 指向数据的指针
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示数据长度
 * + (O) 作为输出时，返回剩余未计算的数据长度，当返回不为0时，
 *       表示计算未完成，计算结果不会镜像翻转，也不会与xorout异或。
 * @return 错误码
 * @retval XWOK: 没有错误，计算结果有效
 * @retval -EFAULT: 空指针
 * @retval -EOPNOTSUPP: 不支持的多项式
 */
__xwlib_code
xwer_t xwlib_crc32_cal(xwu32_t * crc32, xwu32_t xorout,
                       bool refin, bool refout,
                       xwu32_t plynml, xwu32_t direction,
                       const xwu8_t stream[], xwsz_t * size)
{
        xwer_t rc;
        xwsz_t total, pos;
        xwu32_t res;

        XWOS_VALIDATE((crc32), "nullptr", -EFAULT);
        XWOS_VALIDATE((size), "nullptr", -EFAULT);

        total = *size;
        res = *crc32;
        pos = 0;
        /* 使用硬件计算部分CRC32校验值 */
        rc = soc_crc32_cal(&res, refin, plynml, direction, &stream[0], size);
        if (*size > 0) {
                /* 使用软件计算剩余部分的CRC32校验值 */
                pos = total - *size;
                rc = xwlib_crc32_swcal(&res,
                                       refin, plynml, direction,
                                       &stream[pos], size);
                pos = total - *size;
        }/* else {} */
        if (XWOK == rc) {
                if (0 == *size) {
                        if (refout) {
                                res = xwbop_rbit32(res);
                        }/* else {} */
                        res = res ^ xorout;
                }/* else {} */
                *crc32 = res;
        }/* else {} */
        return rc;
}

/**
 * @brief 用主流CRC32参数模型计算一段数据的CRC32校验值
 * @param[in] stream: 指向数据的指针
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示数据长度
 * + (O) 作为输出时，返回剩余未计算的数据长度
 * @return CRC32
 * @note
 * + 主流CRC32参数模型：
 *   - 多项式：0xEDB88320
 *   - 初始值：0xFFFFFFFF
 *   - 数据移位方向：右移
 *   - 是否按位镜像翻转输入：否
 *   - 是否按位镜像翻转输出：否
 *   - 与结果进行异或计算：0xFFFFFFFF
 * + 主流CRC32参数的镜像模型：
 *   - 多项式：0x04C11DB7
 *   - 初始值：0xFFFFFFFF
 *   - 数据移位方向：左移
 *   - 是否按位镜像翻转输入：是
 *   - 是否按位镜像翻转输出：是
 *   - 与结果进行异或计算：0xFFFFFFFF
 * + 上面两个模型计算结果是一致的；
 * + 本算法优先使用硬件加速运算，但某些平台的硬件可能有字节数的要求
 *   （例如STM32要求字节数为4的倍数），多余的字节再采用软件计算；
 * + 本软件算法使用的是直驱表法。
 */
__xwlib_code
xwu32_t xwlib_crc32_calms(const xwu8_t stream[], xwsz_t * size)
{
        xwu32_t result;

        result = 0xFFFFFFFF;
        xwlib_crc32_cal(&result, 0xFFFFFFFF, false, false,
                        0xEDB88320, XWLIB_CRC32_RIGHT_SHIFT,
                        stream, size);
        return result;
}
