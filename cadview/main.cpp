#include <gtk/gtk.h>
#include "slate.h"

int main(int argc, char *argv[]){
  GtkWidget *window;
  GtkWidget *slate;
  GtkWidget *vbox;
  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), 200, 180);

  slate = gtk_slate_new();

  vbox = gtk_vbox_new(TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), slate, TRUE, TRUE, 3);

  gtk_container_add(GTK_CONTAINER(window), vbox);

  g_signal_connect(G_OBJECT(window), "destroy",
    G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(G_OBJECT(window), "destroy",
    G_CALLBACK(gtk_widget_destroyed), &window);

  cad_core::cad_document<double> document;
  cad_core::cad_gtk_adaptor<double> gui(slate);
  cad_core::cad_gui_view<double,
    cad_core::cad_gtk_adaptor<double> > view(document, gui);
  gtk_slate_set_view(slate, &view);

  gtk_widget_show_all(window);

  gtk_main();

  return 0;
}
