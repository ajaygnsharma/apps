
#ifndef CONVERT_TEMP
#define CONVERT_TEMP

#include <gtk/gtk.h>

#include "main.h"

void ConvertToDegC(GtkWidget *entry, TemperatureData *td);
void SetTemperature(GtkWidget *entry, TemperatureData *td);

#endif
