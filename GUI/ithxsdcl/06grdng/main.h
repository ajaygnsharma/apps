#ifndef MAIN_INCLUDED
#define MAIN_INCLUDED

typedef struct _TemperatureData TemperatureData;

void get_reading(GtkWidget *entry, TemperatureData *td);
void set_ip_address( GtkWidget *entry, TemperatureData *td);

struct _TemperatureData
{
  gchar *ip_address;
  gint  repeat_delay;
  GtkWidget *TemperatureDegC;
};

#endif
