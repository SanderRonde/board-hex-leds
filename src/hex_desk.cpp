#include <hex.h>
#include <stdlib.h>
#include <string.h>

const hex_describer_t described_hexes_desk[] = {
		{.id = 1, .offset_percentage = 0, .invert_order = false, .borders = {-1, 2, 8, 7, 18, 6}, .leds = {262, 263, 264, 265, 266, 267, 268, 269, 270, 271, 272, 273, 274, 275, 276, 277, 278, 279, 280, 281, 282, 283, 284, 285, 286, 287, 288}},
		{.id = 2, .offset_percentage = 0, .invert_order = false, .borders = {3, 10, 9, 8, 1, -1}, .leds = {45, 46, 47, 48, 49, 430, 431, 432, 433, 434, 435, 436, 437, 438, 439, 440, 441, 442, 443, 444, 445, 446, 447, 448, 449, 450, 451}},
		{.id = 3, .offset_percentage = 0, .invert_order = false, .borders = {12, 11, 10, 2, -1, 4}, .leds = {72, 73, 74, 75, 76, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412, 413, 414, 415, 416, 417, 418, 419, 420, 421, 422, 423, 424}},
		{.id = 4, .offset_percentage = 0, .invert_order = false, .borders = {13, 12, 3, -1, 5, 14}, .leds = {344, 345, 346, 347, 348, 349, 350, 351, 352, 353, 354, 355, 356, 357, 358, 359, 360, 361, 362, 363, 364, 365, 366, 367, 368, 369, 370}},
		{.id = 5, .offset_percentage = 0, .invert_order = false, .borders = {14, 4, -1, 6, 16, 15}, .leds = {167, 168, 169, 170, 307, 308, 309, 310, 311, 312, 313, 314, 315, 316, 317, 318, 319, 320, 321, 322, 323, 324, 325, 326, 327, 328, 329}},
		{.id = 6, .offset_percentage = 0, .invert_order = false, .borders = {5, -1, 1, 18, 17, 16}, .leds = {194, 195, 196, 197, 198, 253, 254, 255, 256, 257, 258, 259, 260, 261, 289, 290, 291, 292, 293, 294, 295, 296, 297, 298, 299, 300, 301, 302}},
		{.id = 7, .offset_percentage = 0, .invert_order = false, .borders = {1, 8, -1, -1, -1, 18}, .leds = {0, 1, 2, 3, 4, 5, 6, 7, 8, 471, 472, 473, 474, 475, 476, 477, 478, 479, 480, 481, 482, 483, 484, 485, 486, 487, 488}},// 489, 490, 491, 492, 493, 494, 495, 496}},
		{.id = 8, .offset_percentage = 0, .invert_order = false, .borders = {2, 9, -1, -1, 7, 1}, .leds = {9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 457, 458, 459, 460, 461, 462, 463, 464, 465, 466, 467, 468, 469, 470}},
		{.id = 9, .offset_percentage = 0, .invert_order = false, .borders = {10, -1, -1, -1, 8, 2}, .leds = {22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 452, 453, 454, 455, 456}},
		{.id = 10, .offset_percentage = 0, .invert_order = false, .borders = {11, -1, -1, 9, 2, 3}, .leds = {50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 425, 426, 427, 428, 429}},
		{.id = 11, .offset_percentage = 0, .invert_order = false, .borders = {-1, -1, -1, 10, 3, 12}, .leds = {77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 398, 399, 400, 401, 402}},
		{.id = 12, .offset_percentage = 0, .invert_order = false, .borders = {-1, -1, 11, 3, 4, 13}, .leds = {99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 384, 385, 386, 387, 388, 389, 390, 391, 392, 393, 394, 395, 396, 397}},
		{.id = 13, .offset_percentage = 0, .invert_order = false, .borders = {-1, -1, 12, 4, 14, -1}, .leds = {113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 375, 376, 377, 378, 379, 380, 381, 382, 383}},
		{.id = 14, .offset_percentage = 0, .invert_order = false, .borders = {-1, 13, 4, 5, 15, -1}, .leds = {131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 335, 336, 337, 338, 339, 340, 341, 342, 343, 371, 372, 373, 374}},
		{.id = 15, .offset_percentage = 0, .invert_order = false, .borders = {-1, 14, 5, 16, -1, -1}, .leds = {144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 330, 331, 332, 333, 334}},
		{.id = 16, .offset_percentage = 0, .invert_order = false, .borders = {15, 5, 6, 17, -1, -1}, .leds = {171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 303, 304, 305, 306}},
		{.id = 17, .offset_percentage = 0, .invert_order = false, .borders = {16, 6, 18, -1, -1, -1}, .leds = {199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 249, 250, 251, 252}},
		{.id = 18, .offset_percentage = 0, .invert_order = false, .borders = {6, 1, 7, -1, -1, 17}, .leds = {221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248}}};

size_t num_hexes_desk = sizeof(described_hexes_desk) / sizeof(hex_describer_t);
