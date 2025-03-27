#pragma once
#include "gameComponent.hpp"
#include "render/render.hpp"
#include "engine/data/models.hpp"

class RenderComponent : public GameComponent{
protected:
	std::shared_ptr<RenderObject> renderObject{};
public:
	RenderComponent(Models::Shape s);
protected:
	bool active;

	void onUpdate(Context c) override;

	void onFixedUpdate(FixedContext c) override;

	InputResult onEvent(const InputEvent &event) override;
};
