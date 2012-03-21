#ifndef S2_H_INCLUDED
#define S2_H_INCLUDED

#include <gtk/gtk.h>

#include "primitives.h"
#include <vector>

template <typename T> class Entities {
public:
  vector< polyline_2d<T> > lines;
};

G_BEGIN_DECLS

#define GTK_TYPE_S2            (gtk_s2_get_type ())
#define GTK_S2(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_S2, GtkS2))
#define GTK_S2_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_S2, GtkS2Class))
#define GTK_IS_S2(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_S2))
#define GTK_IS_S2_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_S2))
#define GTK_S2_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_S2, GtkS2Class))

typedef double sfloat;
typedef struct _GtkS2       GtkS2;
typedef struct _GtkS2Class  GtkS2Class;

struct _GtkS2
{
  GtkWidget widget;

  Entities<sfloat> ents;

  polyline_2d<sfloat> curr_polyline;
};

struct _GtkS2Class
{
  GtkWidgetClass parent_class;

  /* Padding for future expansion */
  void (*_gtk_reserved1) (void);
  void (*_gtk_reserved2) (void);
  void (*_gtk_reserved3) (void);
  void (*_gtk_reserved4) (void);
};


GType      gtk_s2_get_type   (void) G_GNUC_CONST;
GtkWidget* gtk_s2_new        (void);

#ifndef GTK_DISABLE_DEPRECATED
void       gtk_s2_size       (GtkS2      *s2,
					gint                 width,
					gint                 height);
#endif /* GTK_DISABLE_DEPRECATED */


G_END_DECLS

#endif // S2_H_INCLUDED
