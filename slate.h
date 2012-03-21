#ifndef __GTK_SLATE_H__
#define __GTK_SLATE_H__


#include <gtk/gtk.h>

#include "../cadcore/cad_gui_view.h"
#include "../cadcore/cad_gtk_adaptor.h"

#include <vector>

//template <typename T> class Entities {
//public:
//  vector< polyline_2d<T> > lines;
//};

G_BEGIN_DECLS

#define GTK_TYPE_SLATE            (gtk_slate_get_type ())
#define GTK_SLATE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_SLATE, GtkSlate))
#define GTK_SLATE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_SLATE, GtkSlateClass))
#define GTK_IS_SLATE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_SLATE))
#define GTK_IS_SLATE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_SLATE))
#define GTK_SLATE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_SLATE, GtkSlateClass))

typedef double sfloat;
typedef struct _GtkSlate       GtkSlate;
typedef struct _GtkSlateClass  GtkSlateClass;

using cad_core::cad_document;
using cad_core::polyline_2d;
using cad_core::point_2d;

using std::vector;

typedef cad_core::cad_gui_view<double,
    cad_core::cad_gtk_adaptor<double> > view_type;

struct _GtkSlate
{
  GtkWidget widget;
  view_type* view;
  gboolean is_panning;
  double pan_start_x;
  double pan_start_y;
  polyline_2d<sfloat> curr_polyline;
};

struct _GtkSlateClass
{
  GtkWidgetClass parent_class;

  /* Padding for future expansion */
  void (*_gtk_reserved1) (void);
  void (*_gtk_reserved2) (void);
  void (*_gtk_reserved3) (void);
  void (*_gtk_reserved4) (void);
};


GType      gtk_slate_get_type   (void) G_GNUC_CONST;
GtkWidget* gtk_slate_new        (void);
void gtk_slate_set_view(GtkWidget *widget, view_type *view);
void gtk_slate_device_to_user(GtkWidget *widget, double *x, double *y);

#ifndef GTK_DISABLE_DEPRECATED
void       gtk_slate_size       (GtkSlate      *slate,
					gint                 width,
					gint                 height);
#endif /* GTK_DISABLE_DEPRECATED */


G_END_DECLS


#endif /* __GTK_SLATE_H__ */
