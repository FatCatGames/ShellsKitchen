#pragma once

enum IngredientType
{
	Fish,
	Shrimp,
	Rice,
	Squid,
	Seaweed,
	Caviar,
	COUNT
};

inline std::string IngredientTypeToName(IngredientType anIngredientType)
{
	switch (anIngredientType)
	{
	case Fish: return "Fish";
	case Shrimp: return "Shrimp";
	case Rice: return "Rice";
	case Squid: return "Squid";
	case Seaweed: return "Seaweed";
	case Caviar: return "Caviar";
	return "Undefined";
	}
}