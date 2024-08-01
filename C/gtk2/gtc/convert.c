#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"

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

  sprintf(result_str,"%f", result);

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
