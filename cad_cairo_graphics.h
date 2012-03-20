#ifndef CAD_CAIRO_GRAPHICS_H_INCLUDED
#define CAD_CAIRO_GRAPHICS_H_INCLUDED

#include "cad_primitives.h"

namespace cad_core {

template<typename T> class cad_gtk_adaptor;

template<typename T> class cad_cairo_graphics {
  cairo_t *cr;
public:
  cad_cairo_graphics(cad_gtk_adaptor<T> &gui) {
    cr = gdk_cairo_create(gui.get_widget()->window);
    cairo_set_line_width(cr, 1.0);

    cairo_save(cr);
  }

  void init(T scale, T translate_x, T translate_y){
    cairo_matrix_t mx;
    cairo_matrix_init_identity(&mx);
    cairo_matrix_scale(&mx, scale, scale);
    cairo_matrix_translate(&mx, translate_x, translate_y);
    cairo_set_matrix(cr, &mx);

    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
  }

  void move_to(const T x, const T y){
    cairo_move_to(cr, x, y);
  }

  void move_to(const point_2d<T>& p){
    move_to(p.x, p.y);
  }

  void line_to(const T x, const T y){
    cairo_line_to(cr, x, y);
  }

  void line_to(const point_2d<T>& p){
    line_to(p.x, p.y);
  }

  void draw_polyline(const polyline_2d<T>& polyline){
    const vector< point_2d<T> >& points = polyline.points;
    move_to(points[0]);

    for(int ipoint = 1, npoint = points.size();
      ipoint < npoint; ++ipoint)
      line_to(points[ipoint]);

  }

  void device_to_user(T *x, T *y){
    cairo_device_to_user(cr, x, y);
  }

  ~cad_cairo_graphics(){
    cairo_restore(cr);
    cairo_stroke(cr);
    cairo_destroy(cr);
  }
};

}

#endif // CAD_CAIRO_GRAPHICS_H_INCLUDED
