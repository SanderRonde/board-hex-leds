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

HexNS::Hexes *HexNS::Hexes::from_json(String json)
{
	DynamicJsonDocument doc(MAX_JSON_DOC_SIZE);
	DeserializationError error = deserializeJson(doc, json);

	if (error)
	{
		LOGF("Failed to deserialize JSON: %s\n", error.f_str());
		return;
	}

	JsonArray json_hexes = doc["hexes"];
	size_t num_hexes = json_hexes.size();

	Hex **created_hexes = (Hex **)malloc(sizeof(Hex *) * num_hexes);
	for (int i = 0; i < num_hexes; i++)
	{
		created_hexes[i] = Hex::from_json(json_hexes[i]);
	}

	Hexes hexes_cls = Hexes(created_hexes, num_hexes);
	for (int i = 0; i < num_hexes; i++)
	{
		(*created_hexes[i]).parent = &hexes_cls;
	}

	return &hexes_cls;
}