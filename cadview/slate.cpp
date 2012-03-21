
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
}

GtkWidget*
gtk_slate_new (void)
{
  return GTK_WIDGET(g_object_new (GTK_TYPE_SLATE, NULL));
}

void gtk_slate_set_view(GtkWidget *widget, view_type *view){
  GtkSlate *slate = GTK_SLATE(widget);
  slate->view = view;
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

  slate->view->paint();

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

  GdkModifierType mods;
  gint x, y;
  if(event->is_hint)
    gdk_window_get_pointer(widget->window, &x, &y, &mods);
  else {
    x = event->x;
    y = event->y;
  }

  slate->view->mouse_move(x, y);

  return FALSE;
}

cad_core::Mouse_button Mouse_button_from_Gtk(guint gtk_mouse_button){
  switch(gtk_mouse_button){
    case 1: return cad_core::Mouse_button_Left;
    case 2: return cad_core::Mouse_button_Middle;
    case 3: return cad_core::Mouse_button_Right;
    default: return cad_core::Mouse_button_None;
  }
}

gboolean gtk_slate_button_press (GtkWidget	     *widget,
         GdkEventButton      *event){
  GtkSlate *slate = GTK_SLATE(widget);
  cad_core::Mouse_button button = Mouse_button_from_Gtk(event->button);
  slate->view->mouse_down(button, event->x, event->y);

  return TRUE;
}

gboolean gtk_slate_button_release (GtkWidget	     *widget,
         GdkEventButton      *event){

  GtkSlate *slate = GTK_SLATE(widget);
  cad_core::Mouse_button button = Mouse_button_from_Gtk(event->button);
  slate->view->mouse_up(button, event->x, event->y);

  return TRUE;
}

cad_core::Mouse_scroll_direction Mouse_scroll_direction_from_Gtk(
  GdkScrollDirection gdk_scroll_direction){

  switch(gdk_scroll_direction){
    case GDK_SCROLL_UP:
      return cad_core::Mouse_scroll_direction_Up;
    case GDK_SCROLL_DOWN:
      return cad_core::Mouse_scroll_direction_Down;
    default:
      return cad_core::Mouse_scroll_direction_None;
  }

}

gboolean gtk_slate_scroll_event(GtkWidget           *widget,
					 GdkEventScroll      *event){

  GtkSlate *slate = GTK_SLATE(widget);
  slate->view->mouse_scroll(
    Mouse_scroll_direction_from_Gtk(event->direction),
    event->x, event->y);

  return FALSE;
}
