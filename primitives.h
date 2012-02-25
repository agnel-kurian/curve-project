#ifndef PRIMITIVES_H_INCLUDED
#define PRIMITIVES_H_INCLUDED

#include <vector>
using std::vector;

template <typename T> struct point_2d {
  T x;
  T y;

  inline point_2d(T x, T y) : x(x), y(y) {
  }

  inline point_2d() : point_2d(T(0.0), T(0.0)){
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

#endif // PRIMITIVES_H_INCLUDED
