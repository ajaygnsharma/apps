#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <gtk/gtk.h>
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
  gchar *result_str;

  int sockfd;
  int len;
  struct sockaddr_in address;
  int result;
  char ch[] = "*SRH\r\n";
  char reply[ 20 ];
    
    memset( reply, NULL, sizeof( reply ) );

/*  Create a socket for the client.  */

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

/*  Name the socket, as agreed with the server.  */

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("65.241.1.154");
    address.sin_port = htons(2000);
    len = sizeof(address);

/*  Now connect our socket to the server's socket.  */

    result = connect(sockfd, (struct sockaddr *)&address, len);

    if(result == -1) {
        perror("oops: client3");
        exit(1);
    }

/*  We can now read/write via sockfd.  */

    write(sockfd, ch, sizeof( ch ) );
    read(sockfd, reply, 6);
    printf("H_%s_%%\n", reply);
    
    close(sockfd);
 
  result_str = (gchar *)malloc(100); 

  sprintf(result_str,"%s", reply);

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
