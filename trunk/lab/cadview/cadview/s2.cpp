
#include "s2.h"

static void gtk_s2_realize       (GtkWidget           *widget);
static void gtk_s2_size_allocate (GtkWidget           *widget,
                                     GtkAllocation       *allocation);
static gboolean gtk_s2_expose    (GtkWidget           *widget,
                                     GdkEventExpose      *event);
static void gtk_s2_send_configure (GtkS2           *s2);
static gboolean gtk_s2_motion_notify(GtkWidget        *widget,
                                        GdkEventMotion         *event);
static gboolean gtk_s2_button_release (GtkWidget	     *widget,
                                      GdkEventButton      *event);

G_DEFINE_TYPE (GtkS2, gtk_s2, GTK_TYPE_WIDGET)

static void
gtk_s2_class_init (GtkS2Class *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  widget_class->realize = gtk_s2_realize;
  widget_class->size_allocate = gtk_s2_size_allocate;
  widget_class->expose_event = gtk_s2_expose;
  widget_class->motion_notify_event = gtk_s2_motion_notify;
  widget_class->button_release_event = gtk_s2_button_release;
  fprintf(stderr, "button_release_event set\n");
}

static void
gtk_s2_init (GtkS2 *s2)
{
}

GtkWidget*
gtk_s2_new (void)
{
  return GTK_WIDGET(g_object_new (GTK_TYPE_S2, NULL));
}

void
gtk_s2_size (GtkS2 *s2,
		       gint            width,
		       gint            height)
{
  g_return_if_fail (GTK_IS_S2 (s2));

  GTK_WIDGET (s2)->requisition.width = width;
  GTK_WIDGET (s2)->requisition.height = height;

  gtk_widget_queue_resize (GTK_WIDGET (s2));
}

static void
gtk_s2_realize (GtkWidget *widget)
{
  GtkS2 *s2 = GTK_S2 (widget);
  GdkWindowAttr attributes;
  gint attributes_mask;

  if (!gtk_widget_get_has_window (widget))
    {
      GTK_WIDGET_CLASS (gtk_s2_parent_class)->realize (widget);
    }
  else
    {
      gtk_widget_set_realized (widget, TRUE);

      attributes.window_type = GDK_WINDOW_CHILD;
      attributes.x = widget->allocation.x;
      attributes.y = widget->allocation.y;
      attributes.width = widget->allocation.width;
      attributes.height = widget->allocation.height;
      attributes.wclass = GDK_INPUT_OUTPUT;
      attributes.visual = gtk_widget_get_visual (widget);
      attributes.colormap = gtk_widget_get_colormap (widget);
      attributes.event_mask =
        gtk_widget_get_events (widget) |
        GDK_EXPOSURE_MASK | GDK_POINTER_MOTION_MASK |
        GDK_POINTER_MOTION_HINT_MASK |
        GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK;
      fprintf(stderr, "button release mask set\n");
      attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

      widget->window = gdk_window_new (gtk_widget_get_parent_window (widget),
                                       &attributes, attributes_mask);
      gdk_window_set_user_data (widget->window, s2);

      widget->style = gtk_style_attach (widget->style, widget->window);
      gtk_style_set_background (widget->style, widget->window, GTK_STATE_NORMAL);
    }

  gtk_s2_send_configure (GTK_S2 (widget));
}

static void
gtk_s2_size_allocate (GtkWidget     *widget,
				GtkAllocation *allocation)
{
  g_return_if_fail (GTK_IS_S2 (widget));
  g_return_if_fail (allocation != NULL);

  widget->allocation = *allocation;

  if (gtk_widget_get_realized (widget))
    {
      if (gtk_widget_get_has_window (widget))
        gdk_window_move_resize (widget->window,
                                allocation->x, allocation->y,
                                allocation->width, allocation->height);

      gtk_s2_send_configure (GTK_S2 (widget));
    }
}

static gboolean
gtk_s2_expose(GtkWidget *widget,
    GdkEventExpose *event)
{
  cairo_t *cr;
  cr = gdk_cairo_create(widget->window);

  cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
  cairo_set_line_width(cr, 1.0);

  GtkS2 *s2 = (GtkS2*) widget;
  vector< point_2d<sfloat> >& points = s2->curr_polyline.points;
  if(points.size() == 0){
    cairo_move_to(cr, 0.0, 0.0);
    cairo_line_to(cr, widget->allocation.width, widget->allocation.height);
    cairo_stroke(cr);
  }
  else {
    point_2d<sfloat>& p0 = points[0];
    cairo_move_to(cr, (double)p0.x, (double)p0.y);
    for(int ipoint = 1, npoint = points.size();
      ipoint < npoint; ++ipoint){

      point_2d<sfloat>& pi = points[ipoint];
      cairo_line_to(cr, (double)pi.x, (double)pi.y);

    }

    cairo_stroke(cr);

  }

  cairo_destroy(cr);

  return FALSE;
}

static void
gtk_s2_send_configure (GtkS2 *s2)
{
  GtkWidget *widget;
  GdkEvent *event = gdk_event_new (GDK_CONFIGURE);

  widget = GTK_WIDGET (s2);

  event->configure.window = GDK_WINDOW(g_object_ref (widget->window));
  event->configure.send_event = TRUE;
  event->configure.x = widget->allocation.x;
  event->configure.y = widget->allocation.y;
  event->configure.width = widget->allocation.width;
  event->configure.height = widget->allocation.height;

  gtk_widget_event (widget, event);
  gdk_event_free (event);
}

static gboolean
gtk_s2_motion_notify(GtkWidget        *widget,
  GdkEventMotion         *event)
{
  GdkModifierType state;
  int x, y;
  if(event->is_hint)
    gdk_window_get_pointer(event->window, &x, &y, &state);
  else {
    x = event->x;
    y = event->y;
    state = (GdkModifierType) event->state;
  }

  return FALSE;
}

gboolean gtk_s2_button_release (GtkWidget	     *widget,
         GdkEventButton      *event){
  fprintf(stderr, "button released\n");

  GtkS2 *s2 = (GtkS2*) widget;

  fprintf(stderr, "button released\n");
  cairo_t *cr;
  cr = gdk_cairo_create(widget->window);
  double dx = event->x, dy = event->y;
  cairo_device_to_user(cr, &dx, &dy);
  s2->curr_polyline.points.push_back(point_2d<sfloat>((sfloat)dx, (sfloat)dy));
  cairo_destroy(cr);
  gdk_window_invalidate_rect(widget->window, &widget->allocation, FALSE);

  return TRUE;
}
