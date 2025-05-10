#pragma once
#include "../component.hpp"

class UiComponent : public Component {
protected:
	bool checkValidity(const Pawn& p) override;
};