#include "uiComponent.hpp"
#include "engine/entity/pawns/surfacePawn.hpp"

/*
 * UiComponent
 */

bool UiComponent::checkValidity(const Pawn& p) {
	if (dynamic_cast<const SurfacePawn*>(&p)) {
		return true;
	}
	else {
		printf("ERROR: object %s does not inherit \"SurfacePawn\" whitch is required by component %s", p.getPawnName().c_str(), this->getComponentName().c_str()); //maybe make some function that doest this with arguments in base component 
		return false;
	}
	return true;
}