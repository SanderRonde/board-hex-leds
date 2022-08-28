#include "effects.h"
#include "constants.h"
#include "util.h"
#include "effects/change.h"

namespace Change
{
	void bump_progress(change_t *change, int progress_amount)
	{
		change->progress += progress_amount;
	}

	change_t create_locked(CRGB color)
	{
		change_t change;
		change.locked = true;
		change.color = color;
		return change;
	}

	change_t set_new_color_target(change_t prev_change, int interval_min, int interval_max)
	{
		change_t change;
		change.locked = false;
		change.current = prev_change.next;
		change.progress = 0;
		change.total = random(interval_min, interval_max);
		change.next = random(0, MAX_CSHV_VALUE);
		return change;
	}

	int get_current_value(change_t change)
	{
		int diff = change.next - change.current;
		double progress = Util::divide(change.progress, change.total);
		return change.current + (diff * progress);
	}

	CRGB get_current_color(change_t change)
	{
		if (change.locked)
		{
			Serial.printf("Locked\n");
			return change.color;
		}
		CRGB current = CHSV(change.current, MAX_CSHV_VALUE, MAX_CSHV_VALUE);
		CRGB next = CHSV(change.next, MAX_CSHV_VALUE, MAX_CSHV_VALUE);
		double progress = Util::divide(change.progress, change.total);
		return Util::fade_towards_color(current, next, progress * MAX_CSHV_VALUE);
	}
}