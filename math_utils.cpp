#include "math_utils.h"


namespace MathUtils {
    const unsigned int getNbVertex(const vertex vertices[]) {
        return (unsigned int)(sizeof(*vertices) / VERTEX_ELEMENTS_NB);
    }

    const unsigned int getNbElements(const vertex vertices[]) {
        return getNbVertex(vertices) * VERTEX_ELEMENTS_NB;
    }
}
