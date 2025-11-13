#pragma once
#include "game.hpp"
#include "render/render.hpp"
#include "engine/data/models.hpp"

class RenderComponent : public GameComponent {
protected:
	std::shared_ptr<RenderObject> rtx_render_object;
	std::shared_ptr<RenderObject> raster_render_object;
	bool last_known_rtx_mode; //TODO temporary (to fix)

public:
	RenderComponent(SpatialPawn* sp, const std::shared_ptr<RenderModel>& renderModel);

    RenderComponent(SpatialPawn* sp, std::string path);

	~RenderComponent() override;

    std::shared_ptr<RenderObject> getRenderObject();

protected:
	bool rendering;


	void onUpdate(Context c) override;

	void onFixedUpdate(FixedContext c) override;

	void onConnected() override;

	InputResult onEvent(const InputEvent& event) override;

	void setRendering(bool is_rendering);

	void remove() override;
};
