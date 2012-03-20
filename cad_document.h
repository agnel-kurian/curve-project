#ifndef CAD_DOCUMENT_H_INCLUDED
#define CAD_DOCUMENT_H_INCLUDED

#include "cad_primitives.h"

namespace cad_core {

template<typename T> class cad_document {
private:
  vector< polyline_2d<T> > polylines;
  vector< point_2d<T> > points;
  vector< line_2d<T> > lines;

public:
  const vector< polyline_2d<T> >& get_polylines() {
    return polylines;
  }

  int add_polyline(const polyline_2d<T>& polyline){
    polylines.push_back(polyline);
    return polylines.size() - 1;
  }

  polyline_2d<T>& get_polyline(int ipolyline){
    return polylines[ipolyline];
  }
};

}

#endif // CAD_DOCUMENT_H_INCLUDED
