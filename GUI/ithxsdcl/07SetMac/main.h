#ifndef MAIN_INC
#define MAIN_INC

typedef struct _MACWidgetData MACWidgetData;

void sendMACAddr( GtkWidget *entry, MACWidgetData *md);
void setMACAddr( GtkWidget *entry, MACWidgetData *md);

struct _MACWidgetData
{
  gchar 	*ipAddr;
  gchar 	*macAddr;
  GtkWidget *MACWidget;
};

#endif
