#pragma once

#include "external.hpp"
#include "render/system.hpp"
#include "render/api/vertex.hpp"
#include "render/api/mesh.hpp"
#include "render/model/obj.hpp"

class Importer {

	public:

		// TODO shouldn't this just be a method on RenderSystem? we need render system here anyway
		static std::vector<std::shared_ptr<RenderMesh>> importObj(RenderSystem& system, const std::string& path);

};
