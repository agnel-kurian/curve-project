#ifndef CAD_CAIRO_GRAPHICS_H_INCLUDED
#define CAD_CAIRO_GRAPHICS_H_INCLUDED

#include "cad_primitives.h"

namespace cad_core {

template<typename T> class cad_gtk_adaptor;

template<typename T> class cad_cairo_graphics {
  cairo_t *cr;
public:
  typedef cairo_matrix_t matrix_type;

  cad_cairo_graphics(cad_gtk_adaptor<T> &gui) {
    cr = gdk_cairo_create(gui.get_widget()->window);
    cairo_set_line_width(cr, 1.0);

    cairo_save(cr);
  }

  void set_matrix(T scale, T translate_x, T translate_y){
    cairo_matrix_t mx;
    cairo_matrix_init_identity(&mx);
    cairo_matrix_scale(&mx, scale, scale);
    cairo_matrix_translate(&mx, translate_x, translate_y);
    cairo_set_matrix(cr, &mx);
  }

  void begin_paint(T scale, T translate_x, T translate_y){
    set_matrix(scale, translate_x, translate_y);
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
    double dx = *x;
    double dy = *y;
    cairo_device_to_user(cr, &dx, &dy);
    *x = (T)dx;
    *y = (T)dy;
  }

  static void set_matrix(cairo_matrix_t *mx,
    double scale, double translate_x, double translate_y){

    cairo_matrix_init_identity(mx);
    cairo_matrix_scale(mx, scale, scale);
    cairo_matrix_translate(mx, translate_x, translate_y);

  }

  static void invert_matrix(cairo_matrix_t *mx){
    cairo_matrix_invert(mx);
  }

  static void transform_point(const cairo_matrix_t *mx, T* x, T* y){
    double dx = *x;
    double dy = *y;

    cairo_matrix_transform_point(mx, &dx, &dy);

    *x = (T)dx;
    *y = (T)dy;

  }

  static void transform_point(const cairo_matrix_t *mx, point_2d<T>* p){
    transform_point(mx, &p->x, &p->y);
  }

  void end_paint(){
    cairo_restore(cr);
    cairo_stroke(cr);
  }

  ~cad_cairo_graphics(){
    cairo_destroy(cr);
  }
};

}

#endif // CAD_CAIRO_GRAPHICS_H_INCLUDED
