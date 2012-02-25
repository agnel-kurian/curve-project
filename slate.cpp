
#include "slate.h"

static void gtk_slate_realize       (GtkWidget           *widget);
static void gtk_slate_size_allocate (GtkWidget           *widget,
                                     GtkAllocation       *allocation);
static gboolean gtk_slate_expose    (GtkWidget           *widget,
                                     GdkEventExpose      *event);
static void gtk_slate_send_configure (GtkSlate           *slate);
static gboolean gtk_slate_motion_notify(GtkWidget        *widget,
                                        GdkEventMotion         *event);
static gboolean gtk_slate_button_release (GtkWidget	     *widget,
                                      GdkEventButton      *event);

G_DEFINE_TYPE (GtkSlate, gtk_slate, GTK_TYPE_WIDGET)

static void
gtk_slate_class_init (GtkSlateClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  widget_class->realize = gtk_slate_realize;
  widget_class->size_allocate = gtk_slate_size_allocate;
  widget_class->expose_event = gtk_slate_expose;
  widget_class->motion_notify_event = gtk_slate_motion_notify;
  widget_class->button_release_event = gtk_slate_button_release;
}

static void
gtk_slate_init (GtkSlate *slate)
{
  slate->slate_data = NULL;
}

GtkWidget*
gtk_slate_new (void)
{
  return GTK_WIDGET(g_object_new (GTK_TYPE_SLATE, NULL));
}

void
gtk_slate_size (GtkSlate *slate,
		       gint            width,
		       gint            height)
{
  g_return_if_fail (GTK_IS_SLATE (slate));

  GTK_WIDGET (slate)->requisition.width = width;
  GTK_WIDGET (slate)->requisition.height = height;

  gtk_widget_queue_resize (GTK_WIDGET (slate));
}

static void
gtk_slate_realize (GtkWidget *widget)
{
  GtkSlate *slate = GTK_SLATE (widget);
  GdkWindowAttr attributes;
  gint attributes_mask;

  if (!gtk_widget_get_has_window (widget))
    {
      GTK_WIDGET_CLASS (gtk_slate_parent_class)->realize (widget);
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

      attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

      widget->window = gdk_window_new (gtk_widget_get_parent_window (widget),
                                       &attributes, attributes_mask);
      gdk_window_set_user_data (widget->window, slate);

      widget->style = gtk_style_attach (widget->style, widget->window);
      gtk_style_set_background (widget->style, widget->window, GTK_STATE_NORMAL);
    }

  gtk_slate_send_configure (GTK_SLATE (widget));
}

static void
gtk_slate_size_allocate (GtkWidget     *widget,
				GtkAllocation *allocation)
{
  g_return_if_fail (GTK_IS_SLATE (widget));
  g_return_if_fail (allocation != NULL);

  widget->allocation = *allocation;

  if (gtk_widget_get_realized (widget))
    {
      if (gtk_widget_get_has_window (widget))
        gdk_window_move_resize (widget->window,
                                allocation->x, allocation->y,
                                allocation->width, allocation->height);

      gtk_slate_send_configure (GTK_SLATE (widget));
    }
}

static gboolean
gtk_slate_expose(GtkWidget *widget,
    GdkEventExpose *event)
{
  cairo_t *cr;
  cr = gdk_cairo_create(widget->window);

  cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
  cairo_set_line_width(cr, 1.0);

  GtkSlate *slate = (GtkSlate*) widget;
  vector< polyline_2d<sfloat> > lines = slate->ents.lines;
  for(int i = 0, n = lines.size(); i < n; ++i){
    vector< point_2d<sfloat> >& points = lines[i].points;
    point_2d<sfloat>& p0 = points[0];
    cairo_move_to(cr, (double)p0.x, (double)p0.y);
    for(int ipoint = 1, npoint = points.size();
      ipoint < npoint; ++ipoint){

      point_2d<sfloat>& pi = points[ipoint];
      cairo_line_to(cr, (double)pi.x, (double)pi.y);

    }

    cairo_stroke(cr);

  }

  vector< point_2d<sfloat> >& points = slate->curr_polyline.points;
  if(points.size() > 0){

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
gtk_slate_send_configure (GtkSlate *slate)
{
  GtkWidget *widget;
  GdkEvent *event = gdk_event_new (GDK_CONFIGURE);

  widget = GTK_WIDGET (slate);

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
gtk_slate_motion_notify(GtkWidget        *widget,
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

gboolean gtk_slate_button_release (GtkWidget	     *widget,
         GdkEventButton      *event){

  GtkSlate *slate = (GtkSlate*) widget;

  if(event->button == 1){
    cairo_t *cr;
    cr = gdk_cairo_create(widget->window);
    double dx = event->x, dy = event->y;
    cairo_device_to_user(cr, &dx, &dy);
    slate->curr_polyline.points.push_back(point_2d<sfloat>((sfloat)dx, (sfloat)dy));
    cairo_destroy(cr);
    gdk_window_invalidate_rect(widget->window, NULL, FALSE);
  }
  else if(event->button == 3){
    if(slate->curr_polyline.points.size() > 1){
      slate->ents.lines.push_back(polyline_2d<sfloat>());
      slate->curr_polyline.points.swap(
                                       slate->ents.lines.
                                       back().points);
      gdk_window_invalidate_rect(widget->window, NULL, FALSE);
    }
  }

  return TRUE;
}
