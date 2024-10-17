#pragma once

namespace MathUtils {
    const unsigned int VERTEX_ELEMENTS_NB = 6;
    typedef float vertex[VERTEX_ELEMENTS_NB];

    inline const float x(const vertex *v) { return (*v)[0]; };
    inline const float y(const vertex *v) { return (*v)[1]; };
    inline const float z(const vertex *v) { return (*v)[2]; };
    inline const float r(const vertex *v) { return (*v)[3]; };
    inline const float g(const vertex *v) { return (*v)[4]; };
    inline const float b(const vertex *v) { return (*v)[5]; };
}
