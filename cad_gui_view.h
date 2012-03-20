#ifndef CAD_GUI_VIEW_H_INCLUDED
#define CAD_GUI_VIEW_H_INCLUDED

#include "cad_document.h"

namespace cad_core {

template <typename T, typename U> class cad_gui_view {

  T translate_x;
  T translate_y;
  T scale;
  bool is_panning;
  point_2d<T> pan_start;

  U gui;

  polyline_2d<T> new_polyline;
  line_2d<T> new_line;

public:
  cad_document<T>& document;

  cad_gui_view(cad_document<T>& document) :
    translate_x(T(0.0)), translate_y(T(0.0)), scale(T(0.0)),
    is_panning(false), pan_start(T(0.0), T(0.0)),
    document(document) {
  }

  void paint(){

    U::graphics_type graphics(gui);

    graphics.init(scale, translate_x, translate_y);

    const vector< polyline_2d<T> >& lines =
      document.get_polylines();

    for(int i = 0, n = lines.size(); i < n; ++i){

      const vector< point_2d<T> >& points = lines[i].points;
      const point_2d<T>& p0 = points[0];
      graphics.move_to(p0.x, p0.y);
      for(int ipoint = 1, npoint = points.size();
        ipoint < npoint; ++ipoint){

        const point_2d<T>& pi = points[ipoint];
        graphics.line_to(pi.x, pi.y);

      }
    }

    //  run through a list of overlays and draw? ...
    //  or...
    vector< point_2d<T> >& points = new_polyline.points;
    if(points.size() > 0){
      graphics.draw_polyline(points);
      point_2d<T>& plast = *points.back();
      graphics.move_to(plast.x, plast.y);
      int x, y;
      gui.get_pointer(&x, &y);
      T dx = (T)x, dy = (T)y;
      graphics.device_to_user(&dx, &dy);
      graphics.line_to(dx, dy);

    }
  }
};

}

#endif // CAD_GUI_VIEW_H_INCLUDED
