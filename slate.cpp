
#include "slate.h"
#include <assert.h>

static void gtk_slate_realize       (GtkWidget           *widget);
static void gtk_slate_size_allocate (GtkWidget           *widget,
                                     GtkAllocation       *allocation);
static gboolean gtk_slate_expose    (GtkWidget           *widget,
                                     GdkEventExpose      *event);
static void gtk_slate_send_configure (GtkSlate           *slate);
static gboolean gtk_slate_motion_notify(GtkWidget        *widget,
                                        GdkEventMotion         *event);
static gboolean gtk_slate_button_press (GtkWidget *widget,
                                        GdkEventButton *event);
static gboolean gtk_slate_button_release (GtkWidget	     *widget,
                                      GdkEventButton      *event);
static gboolean gtk_slate_scroll_event(GtkWidget           *widget,
					 GdkEventScroll      *event);

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
  widget_class->button_press_event = gtk_slate_button_press;
  widget_class->scroll_event = gtk_slate_scroll_event;
}

static void
gtk_slate_init (GtkSlate *slate)
{
  slate->slate_data = NULL;
  slate->scale = 1.0;
  slate->translate_x = 0.0;
  slate->translate_y = 0.0;
  slate->is_panning = FALSE;
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
        GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_SCROLL_MASK;

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
  GtkSlate *slate = GTK_SLATE(widget);
  cairo_t *cr;
  cr = gdk_cairo_create(widget->window);
  cairo_set_line_width(cr, 1.0);

  cairo_save(cr);
  cairo_matrix_t mx;
  cairo_matrix_init_identity(&mx);
  cairo_matrix_scale(&mx, slate->scale, slate->scale);
  cairo_matrix_translate(&mx, slate->translate_x, slate->translate_y);
  cairo_set_matrix(cr, &mx);

  cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);

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

    GdkModifierType mods;
    gint x, y;
    gdk_window_get_pointer(widget->window, &x, &y, &mods);
    double dx = (double)x, dy = (double)y;
    cairo_device_to_user(cr, &dx, &dy);
    cairo_line_to(cr, dx, dy);

  }

  cairo_restore(cr);
  cairo_stroke(cr);
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
  GtkSlate *slate = GTK_SLATE(widget);

  if(slate->is_panning){
    GdkModifierType mods;
    gint x, y;
    if(event->is_hint)
      gdk_window_get_pointer(widget->window, &x, &y, &mods);
    else {
      x = event->x;
      y = event->y;
    }

    double dx = (double)x, dy = (double)y;
    gtk_slate_device_to_user(widget, &dx, &dy);

    slate->translate_x = slate->translate_x + (dx - slate->pan_start_x);
    slate->translate_y = slate->translate_y + (dy - slate->pan_start_y);

    gdk_window_invalidate_rect(widget->window, NULL, FALSE);
  }
  else  if(slate->curr_polyline.points.size() > 0)
    gdk_window_invalidate_rect(widget->window, NULL, FALSE);

  return FALSE;
}

gboolean gtk_slate_button_press (GtkWidget	     *widget,
         GdkEventButton      *event){
  GtkSlate *slate = GTK_SLATE(widget);
  if(event->button == 2){
    slate->is_panning = TRUE;
    slate->pan_start_x = event->x;
    slate->pan_start_y = event->y;
    gtk_slate_device_to_user(widget,
      &slate->pan_start_x, &slate->pan_start_y);

    slate->old_translate_x = slate->translate_x;
    slate->old_translate_y = slate->translate_y;
  }

  return TRUE;
}

void gtk_slate_device_to_user(GtkWidget *widget, double *x, double *y){
  GtkSlate *slate = GTK_SLATE(widget);
  cairo_t *cr;
  cr = gdk_cairo_create(widget->window);
  cairo_matrix_t mx;
  cairo_matrix_init_identity(&mx);
  cairo_matrix_scale(&mx, slate->scale, slate->scale);
  cairo_matrix_translate(&mx, slate->translate_x, slate->translate_y);
  cairo_set_matrix(cr, &mx);
  cairo_device_to_user(cr, x, y);
  cairo_destroy(cr);
}

gboolean gtk_slate_button_release (GtkWidget	     *widget,
         GdkEventButton      *event){

  GtkSlate *slate = GTK_SLATE(widget);
  int npoints;

  if(event->button == 1){
    double dx = event->x, dy = event->y;
    gtk_slate_device_to_user(widget, &dx, &dy);
    slate->curr_polyline.points.push_back(point_2d<sfloat>((sfloat)dx, (sfloat)dy));
    gdk_window_invalidate_rect(widget->window, NULL, FALSE);
  }
  else if(event->button == 2){
    slate->is_panning = FALSE;
  }
  else if(event->button == 3){
    npoints = slate->curr_polyline.points.size();

    if(slate->curr_polyline.points.size() > 1){
      slate->ents.lines.push_back(polyline_2d<sfloat>());
      slate->curr_polyline.points.swap(
         slate->ents.lines.
         back().points);
      gdk_window_invalidate_rect(widget->window, NULL, FALSE);
    }
    else if(npoints == 1){
      slate->curr_polyline.points.clear();
      gdk_window_invalidate_rect(widget->window, NULL, FALSE);
    }

  }
  else
    fprintf(stderr, "button %d\n", event->button);

  return TRUE;
}

gboolean gtk_slate_scroll_event(GtkWidget           *widget,
					 GdkEventScroll      *event){

  GtkSlate *slate = GTK_SLATE(widget);

  cairo_matrix_t mx1;
  cairo_matrix_init_identity(&mx1);
  cairo_matrix_scale(&mx1, slate->scale, slate->scale);
  cairo_matrix_translate(&mx1, slate->translate_x, slate->translate_y);
  cairo_matrix_invert(&mx1);

  double x1 = event->x, y1 = event->y;
  cairo_matrix_transform_point(&mx1, &x1, &y1);

  if(event->direction == GDK_SCROLL_UP){
    slate->scale *= 1.1;
  }
  else if(event->direction == GDK_SCROLL_DOWN){
    slate->scale /= 1.1;
  }
  else
    assert(0);

  cairo_matrix_t mx2;
  cairo_matrix_init_identity(&mx2);
  cairo_matrix_scale(&mx2, slate->scale, slate->scale);
  cairo_matrix_translate(&mx2, slate->translate_x, slate->translate_y);
  cairo_matrix_invert(&mx2);
  double x2 = event->x, y2 = event->y;
  cairo_matrix_transform_point(&mx2, &x2, &y2);
  slate->translate_x += x2 - x1;
  slate->translate_y += y2 - y1;

  gdk_window_invalidate_rect(widget->window, NULL, FALSE);
  return FALSE;
}
