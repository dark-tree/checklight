#pragma once
#include "../component.hpp"

class GameComponent : public Component { // TODO wymyslec lepsza nazwe
protected:

	bool checkValidity(const Pawn& p);
};