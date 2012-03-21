#ifndef CAD_GUI_VIEW_H_INCLUDED
#define CAD_GUI_VIEW_H_INCLUDED

#include "cad_document.h"

#include <cassert>
#include <cstdio>

namespace cad_core {

enum Mouse_scroll_direction {
  Mouse_scroll_direction_None,
  Mouse_scroll_direction_Up,
  Mouse_scroll_direction_Down
};

enum Mouse_button {
  Mouse_button_None,
  Mouse_button_Left,
  Mouse_button_Middle,
  Mouse_button_Right
};

template <typename T, typename U> class cad_gui_view {

  T translate_x;
  T translate_y;
  T scale;
  bool is_panning;
  point_2d<T> pan_start;

  U& gui;

  polyline_2d<T> new_polyline;
  line_2d<T> new_line;

public:
  typedef typename U::graphics_type graphics_type;
  typedef typename U::graphics_type::matrix_type matrix_type;
  cad_document<T>& document;

  cad_gui_view(cad_document<T>& document, U& gui) :
    translate_x(T(0.0)), translate_y(T(0.0)), scale(T(1.0)),
    is_panning(false), pan_start(T(0.0), T(0.0)), gui(gui),
    document(document) {
  }

  void paint(){

    graphics_type graphics(gui);
    graphics.begin_paint(scale, translate_x, translate_y);

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

      graphics.draw_polyline(new_polyline);
      point_2d<T>& plast = points.back();
      graphics.move_to(plast.x, plast.y);
      int x, y;
      gui.get_mouse_position(&x, &y);
      T dx = (T)x, dy = (T)y;
      graphics.device_to_user(&dx, &dy);
      graphics.line_to(dx, dy);

    }

    graphics.end_paint();
  }

  //  mouse move, mouse down, mouse up, mouse wheel
  void mouse_move(int x, int y){

    if(is_panning){
      T dx = (T)x, dy = (T)y;
      graphics_type graphics(gui);
      graphics.set_matrix(scale, translate_x, translate_y);
      graphics.device_to_user(&dx, &dy);
      translate_x += dx - pan_start.x;
      translate_y += dy - pan_start.y;
      gui.invalidate();
    }
    else if(new_polyline.points.size() > 0)
      gui.invalidate();

  }

  void mouse_down(Mouse_button button, int x, int y){
    if(button == Mouse_button_Middle){
      is_panning = true;
      pan_start.x = (T)x;
      pan_start.y = (T)y;
      graphics_type graphics(gui);
      graphics.set_matrix(scale, translate_x, translate_y);
      graphics.device_to_user(&pan_start.x, &pan_start.y);
    }
  }

  void mouse_up(Mouse_button button, int x, int y){
    if(button == Mouse_button_Left){
      graphics_type graphics(gui);
      graphics.set_matrix(scale, translate_x, translate_y);
      T dx = x, dy = y;
      graphics.device_to_user(&dx, &dy);
      new_polyline.points.push_back(point_2d<T>(dx, dy));
      gui.invalidate();
    }
    else if(button == Mouse_button_Middle){
      is_panning = false;
    }
    else if(button == Mouse_button_Right){
      int npoints = new_polyline.points.size();

      if(npoints > 1){
        int ipoly = document.add_polyline(polyline_2d<T>());
        polyline_2d<T>& pline = document.get_polyline(ipoly);
        new_polyline.points.swap(pline.points);
        gui.invalidate();
      }
      else if(npoints == 1){
        new_polyline.points.clear();
        gui.invalidate();
      }

    }
    else
      fprintf(stderr, "button %d\n", button);

  }

  void mouse_scroll(Mouse_scroll_direction direction,
    int x, int y){

    matrix_type mx1;
    graphics_type::set_matrix(&mx1, scale, translate_x, translate_y);
    graphics_type::invert_matrix(&mx1);
    T x1 = x, y1 = y;
    graphics_type::transform_point(&mx1, &x1, &y1);

    if(direction == Mouse_scroll_direction_Up)
      scale *= 1.1;
    else if(direction == Mouse_scroll_direction_Down)
      scale /= 1.1;
    else
      assert(false);

    matrix_type mx2;
    graphics_type::set_matrix(&mx2, scale, translate_x, translate_y);
    graphics_type::invert_matrix(&mx2);
    T x2 = x, y2 = y;
    graphics_type::transform_point(&mx2, &x2, &y2);

    translate_x += x2 - x1;
    translate_y += y2 - y1;

    gui.invalidate();
  }
};

}

#endif // CAD_GUI_VIEW_H_INCLUDED
