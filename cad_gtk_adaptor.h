#ifndef GTK_ADAPTOR_H_INCLUDED
#define GTK_ADAPTOR_H_INCLUDED

#include "gtk/gtk.h"
#include <cassert>

#include "cad_cairo_graphics.h"

namespace cad_core {

template<typename T> class cad_cairo_graphics;

template<typename T> class cad_gtk_adaptor {
  GtkWidget *widget;
public:
  typedef cad_cairo_graphics<T> graphics_type;

  cad_gtk_adaptor(GtkWidget *widget) : widget(widget){
  }

  int get_width(){
    assert(false);
    return 0;
  }

  int get_height(){
    assert(false);
    return 0;
  }

  void create_graphics(graphics_type *graphics){
    cairo_t *cr;
    cr = gdk_cairo_create(widget->window);
    cad_cairo_graphics<T> gr(cr);
    *graphics = gr;
  }

  void get_mouse_position(int *x, int *y){
    GdkModifierType mods;
    gdk_window_get_pointer(widget->window, x, y, &mods);
  }

  GtkWidget* get_widget(){
    return widget;
  }

  void invalidate(){
    gdk_window_invalidate_rect(widget->window, NULL, FALSE);
  }
};

}

#endif // GTK_ADAPTOR_H_INCLUDED
