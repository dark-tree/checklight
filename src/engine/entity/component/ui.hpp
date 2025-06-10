#pragma once
#include "../component.hpp"
#include "../pawns/surfacePawn.hpp"

class UiComponent : public OwnedComponent<SurfacePawn> {
protected:
	UiComponent(SurfacePawn* parent);

	//todo Magistermaks' UI
};
