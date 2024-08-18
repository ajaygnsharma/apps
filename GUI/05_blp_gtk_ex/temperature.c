#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char * password = "secret";

//************************************************************************* 
// closeapp
// 
// Close the application callback function
//
// \param pointer of the window
// \param data passed
//
// \return none
//
//************************************************************************
void closeApp ( GtkWidget *window, gpointer data)
{
  gtk_main_quit();
}

//*************************************************************************
//
// button_clicked
//
// callback function for the button clicked
//
// \param pointer to the button
// \param data passed
//
// \return none
//
//*************************************************************************
void button_clicked (GtkWidget *button, gpointer data)
{
  const char *temperature_text = \
    gtk_entry_get_text(GTK_ENTRY((GtkWidget *)data));
  
  int temperature,t_tmp;

  //
  // Convert data to string
  //
  t_tmp = atoi(temperature_text);
  
  //
  // Calculate the temperature
  //
  temperature = (((t_tmp - 32) * 5)/9);
  
  //
  // Print the temperature
  //
  printf("%d\n",temperature);
}


int main (int argc, char *argv[])
{
  GtkWidget *window;
  GtkWidget *temperature_label;
  GtkWidget *temperature_entry;
  GtkWidget *ok_button;
  GtkWidget *hbox1;
  GtkWidget *vbox;

  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "GtkEntryBox");
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);

  gtk_signal_connect ( GTK_OBJECT (window), "destroy", 
                       GTK_SIGNAL_FUNC ( closeApp), NULL);

  temperature_label = gtk_label_new("Temperature(F)");

  temperature_entry = gtk_entry_new();

  ok_button = gtk_button_new_with_label("Ok");

  gtk_signal_connect (GTK_OBJECT (ok_button), "clicked", 
                      GTK_SIGNAL_FUNC(button_clicked), temperature_entry);

  hbox1 = gtk_hbox_new ( TRUE, 5 );

  vbox = gtk_vbox_new ( FALSE, 10);

  gtk_box_pack_start(GTK_BOX(hbox1), temperature_label, TRUE, FALSE, 5);
  gtk_box_pack_start(GTK_BOX(hbox1), temperature_entry, TRUE, FALSE, 5);

  gtk_box_pack_start(GTK_BOX(vbox), hbox1, FALSE, FALSE, 5);
  gtk_box_pack_start(GTK_BOX(vbox), ok_button, FALSE, FALSE, 5); 
  
  gtk_container_add(GTK_CONTAINER(window), vbox);

  gtk_widget_show_all(window);
  gtk_main ();

  return 0;
}  
