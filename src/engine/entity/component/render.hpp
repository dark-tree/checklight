#pragma once
#include "game.hpp"
#include "render/render.hpp"
#include "engine/data/models.hpp"

class RenderComponent : public GameComponent {
protected:
	std::shared_ptr<RenderObject> render_object;

public:
	RenderComponent(SpatialPawn* sp, Models::Shape s);

	~RenderComponent() override;

protected:
	bool rendering;

	void onUpdate(Context c) override;

	void onFixedUpdate(FixedContext c) override;

	void onConnected() override;

	InputResult onEvent(const InputEvent& event) override;

	void setRendering(bool is_rendering);

	void remove() override;
};
