
#include "vertex.hpp"

/*
 * Vertex3D
 */

Vertex3D::Vertex3D(float x, float y, float z, uint8_t r, uint8_t g, uint8_t b, uint8_t a, float u, float v, uint32_t material_index)
: x(x), y(y), z(z), r(r), g(g), b(b), a(a), u(u), v(v), material_index(material_index) {}
