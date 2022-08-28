#include <hex.h>
#include "constants.h"
#include <map>
#include <vector>

using namespace std;

void get_hex_in_set(int hex_index, std::vector<std::vector<int>> sets, std::vector<int> **out)
{
	for (size_t i = 0; i < sets.size(); i++)
	{
		auto set = sets.at(i);
		for (size_t j = 0; j < set.size(); j++)
		{
			if (set.at(j) == hex_index)
			{
				*out = &set;
				return;
			}
		}
	}
	*out = NULL;
}

int get_hex_distance_from_left(Hex *hex)
{
	Hex *top_neighbour = hex->get_neighbour(HEX_SIDE_TOP_LEFT);
	Hex *bot_neighbour = hex->get_neighbour(HEX_SIDE_BOT_LEFT);
	if (top_neighbour == NULL && bot_neighbour == NULL)
	{
		return 0;
	}

	int max_num = 0;
	if (top_neighbour != NULL)
	{
		max_num = max(max_num, get_hex_distance_from_left(top_neighbour) + 1);
	}
	if (bot_neighbour != NULL)
	{
		max_num = max(max_num, get_hex_distance_from_left(bot_neighbour) + 1);
	}
	return max_num;
}

int get_hex_right_distance_from_left(Hex *hex)
{
	Hex *top_neighbour = hex->get_neighbour(HEX_SIDE_TOP_RIGHT);
	Hex *bot_neighbour = hex->get_neighbour(HEX_SIDE_BOT_RIGHT);
	if (top_neighbour == NULL && bot_neighbour == NULL)
	{
		return get_hex_distance_from_left(hex);
	}
	int max_num = 0;
	if (top_neighbour)
	{
		max_num = max(
				max_num,
				max(
						get_hex_distance_from_left(top_neighbour) - 1,
						get_hex_right_distance_from_left(top_neighbour) - 1));
	}
	if (bot_neighbour)
	{
		max_num = max(
				max_num,
				max(get_hex_distance_from_left(bot_neighbour) - 1,
						get_hex_right_distance_from_left(bot_neighbour) - 1));
	}
	return max_num;
}

int get_distance_from_left(std::vector<int> column, Hexes *hexes)
{
	int max_distance = 0;

	for (size_t i = 0; i < column.size(); i++)
	{
		auto hex = hexes->get_by_index(column.at(i));
		max_distance = max(max_distance, get_hex_right_distance_from_left(hex));
	}

	return max_distance;
}

std::map<int, int> get_hexes_x_positions(Hexes *hexes)
{
	std::vector<std::vector<int>> columns;
	std::vector<int> *vector_out = NULL;
	for (int i = 0; i < hexes->num_hexes; i++)
	{
		auto hex = hexes->get_by_index(i);

		// Make sure it's not already in the array
		get_hex_in_set(hex->index, columns, &vector_out);
		if (vector_out != NULL)
		{
			continue;
		}

		std::vector<int> column;
		column.push_back(hex->index);

		auto hex_above = hex->get_neighbour(HEX_SIDE_TOP);
		while (hex_above != NULL)
		{
			column.push_back(hex_above->index);
			hex_above = hex_above->get_neighbour(HEX_SIDE_TOP);
		}

		auto hex_below = hex->get_neighbour(HEX_SIDE_BOT);
		while (hex_below != NULL)
		{
			column.push_back(hex_below->index);
			hex_below = hex_below->get_neighbour(HEX_SIDE_BOT);
		}

		columns.push_back(column);
	}

	// Now order these columns by the result of `get_distance_from_left`
	std::map<int, int> hex_positions_x;
	for (size_t i = 0; i < columns.size(); i++)
	{
		int distance_from_left = get_distance_from_left(columns.at(i), hexes);
		auto column = columns.at(i);
		for (size_t j = 0; j < column.size(); j++)
		{
			hex_positions_x[column.at(j)] = distance_from_left;
		}
	}

	return hex_positions_x;
}

Hex *get_chained_neighbour(Hex *hex, HEX_SIDE side_1, HEX_SIDE side_2)
{
	Hex *neighbour = hex->get_neighbour(side_1);
	if (neighbour == NULL)
	{
		return NULL;
	}
	return neighbour->get_neighbour(side_2);
}

int get_hex_distance_from_bot(Hex *hex)
{
	Hex *right_neighbour = hex->get_neighbour(HEX_SIDE_BOT_RIGHT);
	Hex *bot_neighbour = hex->get_neighbour(HEX_SIDE_BOT);
	Hex *left_neighbour = hex->get_neighbour(HEX_SIDE_BOT_LEFT);
	if (right_neighbour == NULL && bot_neighbour == NULL && left_neighbour == NULL)
	{
		return 0;
	}

	int max_num = 0;
	if (right_neighbour != NULL)
	{
		max_num = max(max_num, get_hex_distance_from_bot(right_neighbour) + 1);
	}
	if (left_neighbour != NULL)
	{
		max_num = max(max_num, get_hex_distance_from_bot(left_neighbour) + 1);
	}
	if (bot_neighbour != NULL)
	{
		max_num = max(max_num, get_hex_distance_from_bot(bot_neighbour) + 2);
	}
	return max_num;
}

int get_hex_top_distance_from_bot(Hex *hex)
{
	Hex *right_neighbour = hex->get_neighbour(HEX_SIDE_TOP_RIGHT);
	Hex *top_neighbour = hex->get_neighbour(HEX_SIDE_TOP);
	Hex *left_neighbour = hex->get_neighbour(HEX_SIDE_TOP_LEFT);
	if (right_neighbour == NULL && top_neighbour == NULL && left_neighbour == NULL)
	{
		return get_hex_distance_from_bot(hex);
	}

	int max_num = 0;
	if (right_neighbour)
	{
		max_num = max(
				max_num,
				max(get_hex_distance_from_bot(right_neighbour) - 1,
						get_hex_top_distance_from_bot(right_neighbour) - 1));
	}
	if (left_neighbour)
	{
		max_num = max(
				max_num,
				max(get_hex_distance_from_bot(left_neighbour) - 1,
						get_hex_top_distance_from_bot(left_neighbour) - 1));
	}
	if (top_neighbour)
	{
		max_num = max(
				max_num,
				max(
						get_hex_distance_from_bot(top_neighbour) - 2,
						get_hex_top_distance_from_bot(top_neighbour) - 2));
	}

	return max_num;
}

int get_distance_from_bot(std::vector<int> rows, Hexes *hexes)
{
	int max_distance = 0;

	for (size_t i = 0; i < rows.size(); i++)
	{
		auto hex = hexes->get_by_index(rows.at(i));
		max_distance = max(max_distance, get_hex_top_distance_from_bot(hex));
	}

	return max_distance;
}

std::map<int, int> get_hexes_y_positions(Hexes *hexes)
{
	// First build rows
	std::vector<std::vector<int>> rows;
	std::vector<int> *vector_out = NULL;
	for (int i = 0; i < hexes->num_hexes; i++)
	{
		auto hex = hexes->get_by_index(i);

		// Make sure it's not already in the array
		get_hex_in_set(hex->index, rows, &vector_out);
		if (vector_out != NULL)
		{
			continue;
		}

		std::vector<int> row;
		row.push_back(hex->index);
		Hex *hex_right = get_chained_neighbour(hex, HEX_SIDE_TOP_RIGHT, HEX_SIDE_BOT_RIGHT);
		if (hex_right == NULL)
		{
			hex_right = get_chained_neighbour(hex, HEX_SIDE_BOT_RIGHT, HEX_SIDE_TOP_RIGHT);
		}

		while (hex_right != NULL)
		{
			row.push_back(hex_right->index);
			Hex *initial_hex_right = hex_right;
			hex_right = get_chained_neighbour(initial_hex_right, HEX_SIDE_TOP_RIGHT, HEX_SIDE_BOT_RIGHT);
			if (hex_right == NULL)
			{
				hex_right = get_chained_neighbour(initial_hex_right, HEX_SIDE_BOT_RIGHT, HEX_SIDE_TOP_RIGHT);
			}
		}

		Hex *hex_left = get_chained_neighbour(hex, HEX_SIDE_TOP_LEFT, HEX_SIDE_BOT_LEFT);
		if (hex_left == NULL)
		{
			hex_left = get_chained_neighbour(hex, HEX_SIDE_BOT_LEFT, HEX_SIDE_TOP_LEFT);
		}
		while (hex_left != NULL)
		{
			row.push_back(hex_left->index);
			Hex *initial_hex_left = hex_left;
			hex_left = get_chained_neighbour(initial_hex_left, HEX_SIDE_TOP_LEFT, HEX_SIDE_BOT_LEFT);
			if (hex_left == NULL)
			{
				hex_left = get_chained_neighbour(initial_hex_left, HEX_SIDE_BOT_LEFT, HEX_SIDE_TOP_LEFT);
			}
		}

		rows.push_back(row);
	}

	// Now order these rows by the result of `get_distance_from_bot`
	std::map<int, int> hex_positions_y;
	for (size_t i = 0; i < rows.size(); i++)
	{
		auto row = rows.at(i);
		int distance_from_bot = get_distance_from_bot(row, hexes);
		for (size_t j = 0; j < row.size(); j++)
		{
			hex_positions_y[row.at(j)] = distance_from_bot;
		}
	}

	return hex_positions_y;
}

std::map<int, float> get_relative_positions(std::map<int, int> positions)
{
	int max_distance = 0;
	for (const auto &pair : positions)
	{
		max_distance = max(max_distance, pair.second);
	}

	std::map<int, float> relative_postitions;
	for (const auto &pair : positions)
	{
		relative_postitions[pair.first] = (float)pair.second / (float)max_distance;
	}
	return relative_postitions;
}

HexPositions::HexPositions(void *hexes_)
{
	Hexes *hexes = (Hexes *)hexes_;
	hex_positions_x = get_hexes_x_positions(hexes);
	hex_positions_y = get_hexes_y_positions(hexes);
	hex_positions_x_relative = get_relative_positions(hex_positions_x);
	hex_positions_y_relative = get_relative_positions(hex_positions_y);
}