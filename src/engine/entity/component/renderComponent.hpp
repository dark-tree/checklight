#pragma once
#include "gameComponent.hpp"
#include "render/render.hpp"
#include "engine/data/models.hpp"

class RenderComponent : public GameComponent{
protected:
	std::shared_ptr<RenderObject> renderObject;
public:
	explicit RenderComponent(Models::Shape s);

	~RenderComponent() override;
protected:
	bool rendering;

	void onUpdate(Context c) override;

	void onFixedUpdate(FixedContext c) override;

	void onConnected() override;

	InputResult onEvent(const InputEvent &event) override;

	void setRendering(bool is_rendering);

	void remove() override;


};
