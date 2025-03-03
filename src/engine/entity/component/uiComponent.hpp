#pragma once
#include "../component.hpp"

class UiComponent : public Component {
protected:
	virtual bool checkValidity(const Pawn& p) override;
};