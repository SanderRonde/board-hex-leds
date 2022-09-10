#ifdef MOCK

#include "FastLED.a.h"

#define LED_PIN 7

// CRGB
CRGB::CRGB(uint8_t ir, uint8_t ig, uint8_t ib)
{
	r = ir;
	g = ig;
	b = ib;
}
CRGB::CRGB(int colorcode)
{
	r = ((colorcode >> 16) & 0xFF);
	g = ((colorcode >> 8) & 0xFF);
	b = ((colorcode >> 0) & 0xFF);
}
CRGB::CRGB()
{
	r = 0;
	g = 0;
	b = 0;
};

CRGB &CRGB::setHSV(uint8_t hue, uint8_t sat, uint8_t val)
{
	r = 0;
	g = 0;
	b = 0;
	return *this;
}
CRGB &CRGB::nscale8(uint8_t scaledown)
{
	if (scaledown == 0)
	{
		r = 0;
		g = 0;
		b = 0;
	}
	else
	{
		r = r * (scaledown / 256);
		g = g * (scaledown / 256);
		b = b * (scaledown / 256);
	}
	return *this;
}

// FastLed
struct strip_descriptor
{
	struct CRGB *strip;
	int length;
	int offset;
};

struct strip_descriptor strips[MAX_STRIPS];
int strip_index = 0;

void FastLedClass::addLeds(struct CRGB *data, int nLedsOrOffset, int nLedsIfOffset)
{
	strips[strip_index].strip = data;
	strips[strip_index].length = nLedsOrOffset;
	strips[strip_index].offset = nLedsIfOffset;

	strip_index++;
}
void FastLedClass::showColor(const struct CRGB color)
{
	for (int i = 0; i < strip_index; i++)
	{
		for (int j = 0; j < strips[i].length; j++)
		{
			strips[i].strip[strips[i].offset + j] = color;
		}
	}
}
void FastLedClass::showColor(const struct CRGB color, unsigned int intensity)
{
	showColor(color);
	for (int i = 0; i < strip_index; i++)
	{
		for (int j = 0; j < strips[i].length; j++)
		{
			strips[i].strip[strips[i].offset + j].nscale8(intensity);
		}
	}
}
void FastLedClass::show()
{
	// Do nothing
}
void FastLedClass::show(int scale)
{
	for (int i = 0; i < strip_index; i++)
	{
		for (int j = 0; j < strips[i].length; j++)
		{
			strips[i].strip[strips[i].offset + j].nscale8(scale);
		}
	}
}

uint8_t scale8_video( uint8_t i, uint8_t scale) {
	return (((int)i * (int)scale) >> 8) + ((i&&scale)?1:0);
}

uint8_t cos8(uint8_t theta)
{
	return cos(theta);
}

uint8_t sqrt16(uint8_t theta)
{
	return sqrt(theta);
}

FastLedClass FastLED;

#endif