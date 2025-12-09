#ifndef MAIN_INCLUDED
#define MAIN_INCLUDED

typedef struct _TemperatureData TemperatureData;

#include "convert.h"

struct _TemperatureData
{
  gint TemperatureDegF;
  GtkWidget *TemperatureDegC;
};

#endif
