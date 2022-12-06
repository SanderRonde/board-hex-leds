#include <hex.h>
#include <stdlib.h>
#include <string.h>

const hex_describer_t described_hexes_bed[] = {
		{.id = 1,
		 .offset_percentage = 0,
		 .invert_order = true,
		 .borders = {-1, 3, 4, 2, -1, -1},
		 .leds = {234,
							233,
							232,
							231,
							230,
							229,
							228,
							227,
							226,
							225,
							252,
							251,
							250,
							249,
							248,
							247,
							246,
							245,
							244,
							243,
							242,
							241,
							240,
							239,
							238,
							237,
							236,
							235,
							-1}},
		{.id = 2,
		 .offset_percentage = 0,
		 .invert_order = true,
		 .borders = {1, 4, -1, -1, -1, -1},
		 .leds = {261,
							260,
							259,
							258,
							257,
							7,
							6,
							5,
							4,
							3,
							2,
							1,
							0,
							274,
							273,
							272,
							271,
							270,
							269,
							268,
							267,
							266,
							265,
							264,
							263,
							262,
							-1}},
		{.id = 3,
		 .offset_percentage = 0,
		 .invert_order = true,
		 .borders = {-1, -1, 5, 4, 2, -1},
		 .leds = {192,
							191,
							190,
							189,
							188,
							187,
							186,
							185,
							184,
							183,
							210,
							209,
							208,
							207,
							206,
							205,
							204,
							203,
							202,
							201,
							200,
							199,
							198,
							197,
							196,
							195,
							194,
							193, -1}},
		{.id = 4,
		 .offset_percentage = 0,
		 .invert_order = true,
		 .borders = {3, 5, 6, -1, 2, 1},
		 .leds = {
				 220,
				 219,
				 218,
				 217,
				 216,
				 105,
				 104,
				 103,
				 102,
				 101,
				 17,
				 16,
				 15,
				 14,
				 13,
				 12,
				 11,
				 10,
				 9,
				 8,
				 256,
				 255,
				 254,
				 253,
				 224,
				 223,
				 222,
				 221, -1}},
		{.id = 5, .offset_percentage = 0, .invert_order = true, .borders = {-1, 7, 8, 6, 4, 3}, .leds = {178, 177, 176, 175, 174, 118, 117, 116, 115, 114, 113, 112, 111, 110, 109, 108, 107, 106, 215, 214, 213, 212, 211, 182, 181, 180, 179, -1}},
		{.id = 6, .offset_percentage = 0, .invert_order = true, .borders = {5, 8, -1, -1, -1, 4}, .leds = {96, 95, 94, 93, 92, 91, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 100, 99, 98, 97, -1}},
		{.id = 7, .offset_percentage = 40, .invert_order = false, .borders = {-1, 9, 10, 8, 5, -1}, .leds = {119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, -1}},
		{.id = 8, .offset_percentage = 0, .invert_order = true, .borders = {7, 10, -1, -1, 6, 5}, .leds = {82, 81, 80, 79, 78, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 90, 89, 88, 87, 86, 85, 84, 83, -1}},
		{.id = 9, .offset_percentage = 40, .invert_order = false, .borders = {-1, -1, -1, 10, 7, -1}, .leds = {133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, -1}},
		{.id = 10, .offset_percentage = 0, .invert_order = true, .borders = {9, -1, -1, -1, 8, 7}, .leds = {68, 67, 66, 65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 77, 76, 75, 74, 73, 72, 71, 70, 69, -1}}};

size_t num_hexes_bed = sizeof(described_hexes_bed) / sizeof(hex_describer_t);
