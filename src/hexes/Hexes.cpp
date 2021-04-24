#include <hex.h>
#include <telnet.h>

HexNS::Hex *HexNS::Hexes::get_by_id(int id)
{
	for (int i = 0; i < num_hexes; i++)
	{
		if ((*_hexes[i]).id == id)
		{
			return _hexes[i];
		}
	}
	return NULL;
}

HexNS::Hex *HexNS::Hexes::get_by_index(int index)
{
	return _hexes[index];
}

HexNS::Hexes::Hexes(HexNS::Hex **passed_hexes, unsigned int passed_num_hexes)
{
	_hexes = passed_hexes;
	num_hexes = passed_num_hexes;
};

HexNS::Hexes::~Hexes()
{
	free(_hexes);
}

HexNS::Hexes *HexNS::Hexes::from_described(const hex_describer_t hexes[], size_t num_hexes)
{
	Hex **created_hexes = (Hex **)malloc(sizeof(Hex *) * num_hexes);
	for (size_t i = 0; i < num_hexes; i++)
	{
		created_hexes[i] = new Hex(hexes[i]);
	}

	Hexes *hexes_cls = new Hexes(created_hexes, num_hexes);
	for (size_t i = 0; i < num_hexes; i++)
	{
		(*created_hexes[i]).parent = hexes_cls;
		(*created_hexes[i]).index = i;
	}

	return hexes_cls;
}