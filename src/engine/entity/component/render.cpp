#include "render.hpp"
#include "engine/entity/context.hpp"
#include "render/system.hpp"
#include "engine/data/models.hpp"
#include "engine/entity/pawns/spatialPawn.hpp"


RenderComponent::RenderComponent(SpatialPawn* sp, const std::shared_ptr<RenderModel>& renderModel) : GameComponent(sp) {
    // Get the shared model and initial matrix
    auto initial_matrix = glm::identity<glm::mat4x3>();

    rtx_render_object = RenderSystem::system->createRenderObject(true); // true for RT
    rtx_render_object->setMatrix(initial_matrix);
    rtx_render_object->setModel(renderModel);
    rtx_render_object->setActive(true);

    raster_render_object = RenderSystem::system->createRenderObject(false); // false for Raster
    raster_render_object->setMatrix(initial_matrix);
    raster_render_object->setModel(renderModel);
    raster_render_object->setActive(false);

    last_known_rtx_mode = RenderSystem::system->isRTXMode();

    rendering = true;
}

RenderComponent::RenderComponent(SpatialPawn *sp, std::string path) : GameComponent(sp) {
    render_object = RenderSystem::system->createRenderObject();
    render_object->setMatrix(glm::identity<glm::mat4x3>());
    auto obj = RenderSystem::system->importObj(path);
    render_object->setModel(obj[0]);
    render_object->setActive(false);
    rendering = false;
}

void RenderComponent::onUpdate(Context c) {
    glm::mat4x3 matrix = dynamic_cast<SpatialPawn*>(parent)->getMatrix();

    bool mode = RenderSystem::system->isRTXMode();
    if (mode != last_known_rtx_mode)
    {
        last_known_rtx_mode = mode;
        if (mode == true)
        {
            raster_render_object->setActive(false);
            rtx_render_object->setActive(true);
        }
        else if (mode == false)
        {
            raster_render_object->setActive(true);
            rtx_render_object->setActive(false);
        }
    }

    if (rtx_render_object) {
        rtx_render_object->setMatrix(matrix);
    }
    if (raster_render_object) {
        raster_render_object->setMatrix(matrix);
    }
}

void RenderComponent::onFixedUpdate(FixedContext c) {
}

InputResult RenderComponent::onEvent(const InputEvent& event) {
    return InputResult::PASS;
}

void RenderComponent::onConnected() {
    setRendering(true);
}

void RenderComponent::setRendering(bool is_rendering) {
    if(is_rendering != rendering) {
       if (rtx_render_object) {
           rtx_render_object->setActive(is_rendering);
       }
       if (raster_render_object) {
           raster_render_object->setActive(is_rendering);
       }
    }
    rendering = is_rendering;
}

void RenderComponent::remove() {
    GameComponent::remove();

    if (rtx_render_object) {
        rtx_render_object->setActive(false);
    }
    if (raster_render_object) {
        raster_render_object->setActive(false);
    }
}

RenderComponent::~RenderComponent() {
}

std::shared_ptr<RenderObject> RenderComponent::getRenderObject() {
    return render_object;
}


