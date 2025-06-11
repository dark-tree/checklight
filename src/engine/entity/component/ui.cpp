#include "ui.hpp"
#include "engine/entity/pawns/surfacePawn.hpp"
#include "shared/logger.hpp"

/*
 * UiComponent
 */

UiComponent::UiComponent(SurfacePawn* parent) : OwnedComponent<SurfacePawn>(parent) {
}
