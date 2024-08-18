#ifndef MAIN_INCLUDED
#define MAIN_INCLUDED

typedef struct _TemperatureData TemperatureData;

struct _TemperatureData
{
  gint TemperatureDegF;
  GtkWidget *TemperatureDegC;
};

void ConvertToDegC(GtkWidget *entry, TemperatureData *td);
void SetTemperature(GtkWidget *entry, TemperatureData *td);

#endif
