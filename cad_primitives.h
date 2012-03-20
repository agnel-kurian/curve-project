#ifndef CAD_PRIMITIVES_H_INCLUDED
#define CAD_PRIMITIVES_H_INCLUDED

#include <vector>

namespace cad_core {

using std::vector;

template <typename T> struct point_2d {
  T x;
  T y;

  inline point_2d(T x = T(0.0), T y = T(0.0)) : x(x), y(y) {
  }

  inline point_2d(const point_2d<T>& p) : x(p.x), y(p.y) {
  }
};

template <typename T> struct line_2d {
  point_2d<T> start;
  point_2d<T> end;

  inline line_2d() : start(0.0, 0.0), end(1.0, 0.0) {
  }

  inline line_2d(point_2d<T> start, point_2d<T> end) : start(start), end(end) {
  }

  inline line_2d(const line_2d<T>& l) : start(l.start), end(l.end) {
  }
};

template <typename T> struct polyline_2d {
  vector< point_2d<T> > points;
};

}

#endif // CAD_PRIMITIVES_H_INCLUDED
