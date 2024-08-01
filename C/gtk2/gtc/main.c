//*********************************************************************
// main.c 
// 
// Convert a temperature given in Deg F to Deg C
//
// author : ajay
//
//**********************************************************************

#include <gtk/gtk.h>
#include "main.h"

//**********************************************************************
//
// main function
//
// Create the text boxes, submit button etc..
//
// \param argc the no of values
// \param agv the characters in the values
// 
// \return 1
//
//**********************************************************************
int main(int argc, char **argv) 
{
  GtkWidget *window;
  GtkWidget *vbox;
  GtkWidget *entry;
  GtkWidget *frame;
  GtkWidget *convert;

  TemperatureData td = { .TemperatureDegF = 0, .TemperatureDegC = NULL};
  
  //
  // Init the GTK components 
  // Create the top level Window
  // Center the widget
  //
  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);


  //
  // Create the function gtk_main_quit to quit the application when the 
  // [x] button is clicked. and set title
  //
  g_signal_connect(G_OBJECT(window), "delete-event", G_CALLBACK(gtk_main_quit), NULL);
  gtk_window_set_title(GTK_WINDOW(window),"Tmp App");

  
  //
  // Create a vbox with non homogenous layout and spacing of 5 pixels.
  // Add the vbox to the main container 
  //
  vbox = gtk_vbox_new(FALSE, 5);

  gtk_container_add(GTK_CONTAINER(window), vbox);

  gtk_widget_show(vbox);

  
  //
  // Frame that will represent the var 1, 
  // Pack the frame to the vbox 
  // Show the widget
  // Create new entry for the text 
  // Set max length = 10
  // Connect the signal (changed) of object entry to do_sum and pass data
  //   sd structure
  // Add the entry to the frame 
  // Show the entry
  // Show a tooltip for the entry
  //
  frame = gtk_frame_new("Temperature in Ded F");
  
  gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);
  
  gtk_widget_show(frame);

  entry = gtk_entry_new();
  
  gtk_entry_set_max_length(GTK_ENTRY(entry),3);
  
  g_signal_connect(G_OBJECT(entry), "changed", G_CALLBACK(SetTemperature), &td);
  
  gtk_container_add(GTK_CONTAINER(frame), entry);

  gtk_widget_show(entry);
  
  gtk_widget_set_tooltip_text(entry, "Enter the temperature here");  

  // 
  // Create another frame with a label var 2
  // Pack this frame to the vbox
  // Show the frame
  // Create a new text entry 
  // Set max length = 10
  // Connect the entry object's changed event to setvarb
  // Add the entry to the frame
  // Show the entry
  // Add a tooltip
  //
  convert = gtk_button_new_with_label("Convert");
    
  gtk_box_pack_start(GTK_BOX(vbox), convert, FALSE, TRUE,0);
  
  gtk_widget_show(convert);

  g_signal_connect(G_OBJECT(convert), "clicked", G_CALLBACK(ConvertToDegC), &td);
  
  //
  // Create a new message frame 
  // Pack the frame to the vbox
  // Show the frame
  // Create a new label with no text in it
  // Add it to the frame 
  // Show the label
  // Show the window
  //
  frame = gtk_frame_new("Temperature Deg C");

  gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 0);

  gtk_widget_show(frame);

  td.TemperatureDegC = gtk_label_new(NULL);
  
  gtk_container_add(GTK_CONTAINER(frame), td.TemperatureDegC);

  gtk_widget_show(td.TemperatureDegC);

  gtk_widget_show(window);

  //
  // Go to gtk_main() and wait
  // 
  gtk_main();

  return 0;
}
