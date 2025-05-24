#include "ui.hpp"
#include "engine/entity/pawns/surfacePawn.hpp"
#include "shared/logger.hpp"

/*
 * UiComponent
 */

UiComponent::UiComponent(SurfacePawn *parent) : OwnedComponent<SurfacePawn>(parent) {}


// bool UiComponent::checkValidity(const Pawn& p) {
// 	if (dynamic_cast<const SurfacePawn*>(&p)) {
// 		return true;
// 	}
//
// 	out::error("Object %s does not inherit \"SurfacePawn\" which is required by component %s", p.getPawnName().c_str(), this->getComponentName().c_str()); //maybe make some function that doest this with arguments in base component
// 	return false;
// }