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
#include <string.h>
#include <stdlib.h>


//*******************************************************************
// 
// ConvertToDegC
//
// Convert the Temperature to Degree Centigrade
//
// \param Widget Pointer 
// \param Data Pointer
// 
// \return none
//
//******************************************************************
void ConvertToDegC(GtkWidget *entry, TemperatureData *td)
{
  gfloat result;
  gchar *result_str;

  //
  // 1. Get the result of conversion
  // 2. Create space for the result
  // 3. Copy result in float to a buffer after converting to string
  // 4. Set the text of Data pointed by td->TemperatureDegC
  // 5. Clear the buffer
  //
  result = ( ((td->TemperatureDegF - 32) * 5) / 9) ;
  
  result_str = (gchar *)malloc(100); 

  sprintf(result_str,"%2.2f", result);

  gtk_label_set_text(GTK_LABEL(td->TemperatureDegC), result_str);

  g_free(result_str);
}


//*****************************************************************
// SetTemperature
//
// Set the temperature value once it is changed
// 
// \param Pointer to the widget that is holding the data
// \param Temperature data
// 
// \return None
//
//****************************************************************
void SetTemperature(GtkWidget *entry, TemperatureData *td)
{
  const gchar *str;
    
  // 
  // 1. Get the text entry from the widget 
  // 2. Set the TemperatureDegF to the string.
  //
  str = gtk_entry_get_text(GTK_ENTRY(entry));
  td->TemperatureDegF = atof(str);
}

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
  GtkWidget *vbox,*hbox;
  GtkWidget *entry;
  GtkWidget *convert;
  GtkWidget *label;
  //GtkWidget *frame;
  
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
  // Sets the border width of the window.
  // 
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);
    
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
  // Create a hbox 
  // Pack the hbox to the vbox
  // Show it
  // 
  hbox = gtk_hbox_new(FALSE, 5);
  
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
  
  gtk_widget_show (hbox);
  
  
  //
  // Create the label that is asking for temperature in Deg F
  // Pack this label to the hbox.
  // Show it
  //
  label = gtk_label_new ("Temp Deg F");
  
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  
  gtk_widget_show (label);
  
  
  //
  // Create a text entry
  // Set max length  = 3
  // Set a max width = 5
  // Pack it to the hbox
  // Connect signal changed to the settemperature()
  // Show the widget
  // Set a tooltip for this 
  //
  entry = gtk_entry_new();
  
  gtk_entry_set_max_length(GTK_ENTRY(entry),3);
  
  gtk_entry_set_width_chars(GTK_ENTRY(entry),5); 
  
  gtk_box_pack_start (GTK_BOX (hbox), entry, FALSE, FALSE, 0);
  
  g_signal_connect(G_OBJECT(entry), "changed", G_CALLBACK(SetTemperature), &td);
  
  gtk_widget_show(entry);
  
  gtk_widget_set_tooltip_text(entry, "Enter the temperature here");  

  
  
  
  
  //
  // Create a hbox 
  // Pack the hbox to the vbox
  // Show it
  // 
  hbox = gtk_hbox_new(FALSE, 5);
  
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
  
  gtk_widget_show (hbox);
  
  
  //
  // Create the label that is asking for temperature in Deg F
  // Pack this label to the hbox.
  // Show it
  //
  label = gtk_label_new ("Temp Deg C");
  
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  
  gtk_widget_show (label);
  
  
  //
  // Create a new message frame 
  // Pack the frame to the vbox
  // Show the frame
  // Create a new label with no text in it
  // Add it to the frame 
  // Show the label
  // Show the window
  //
  td.TemperatureDegC = gtk_label_new(NULL);
  
  gtk_box_pack_start (GTK_BOX (hbox), td.TemperatureDegC, FALSE, FALSE, 0);
  
  gtk_widget_show(td.TemperatureDegC);

  gtk_widget_show(window);

  
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
  //
  // Create a hbox 
  // Pack the hbox to the vbox
  // Show it
  // 
  hbox = gtk_hbox_new(FALSE, 5);
  
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 5);
  
  gtk_widget_show (hbox);
  
  convert = gtk_button_new_with_label("Convert");
    
  gtk_box_pack_start(GTK_BOX(hbox), convert, TRUE, FALSE,5);
  
  gtk_widget_show(convert);

  g_signal_connect(G_OBJECT(convert), "clicked", G_CALLBACK(ConvertToDegC), &td);
  
  //
  // Go to gtk_main() and wait
  // 
  gtk_main();

  return 0;
}
