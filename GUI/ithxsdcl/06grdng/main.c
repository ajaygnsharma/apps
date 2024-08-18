//*********************************************************************
// main.c 
// 
// get a temperature reading from an iTHX-SD device 
//
// author : ajay
//
//**********************************************************************

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

static int entry_cntr = 0;

//*******************************************************************
// 
// get_reading
//
// Get the reading from the device
//
// \param Widget Pointer 
// \param Data Pointer
// 
// \return none
//
//******************************************************************
void get_reading(GtkWidget *entry, TemperatureData *td)
{
  gchar *result_str;

  int sockfd;
  int len;
  struct sockaddr_in address;
  int result;
//  char ch[] = "*SRTF\r\n";
  char str[50];// = "s -MC00:03:34:00:b8:6f\r\n";
  //char cr[] = "\r\n";
  char reply[ 20 ];
  FILE * pFile;
    
  	memset( ( char *)reply, ( char *)NULL, sizeof( reply ) );

/*  Create a socket for the client.  */

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

/*  Name the socket, as agreed with the server.  */

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("192.168.1.200");
    //address.sin_addr.s_addr = inet_addr(td->ip_address);
    address.sin_port = htons(2002);
    len = sizeof(address);

/*  Now connect our socket to the server's socket.  */

    result = connect(sockfd, (struct sockaddr *)&address, len);

    if(result == -1) {
        perror("oops: client3");
        exit(1);
    }

    /*  We can now read/write via sockfd.  */
    //write(sockfd, ch, sizeof( ch ) );
    	read(sockfd, reply, 10);
	//read(sockfd, reply, 14);
	printf("%s\n", reply);	
	
	strcpy( str, "s -MC" );
	strcat( str,td->ip_address ); 	
	strcat( str, "\r\n");
	printf("mac=%s",td->ip_address );
	write(sockfd, str, strlen( str ) );
	read(sockfd, reply, 18);
	//read(sockfd, reply, 14);
	printf("%s\n", reply);	
	
	write(sockfd, "c\r\n", sizeof( "c\r\n" ) );
	printf("%s\n", reply);	
	read(sockfd, reply, 19);
	//read(sockfd, reply, 14);	
	printf("%s\n", reply);
    
    close(sockfd);
 
    result_str = (gchar *)malloc(100); 

    sprintf(result_str,"%s", reply);

	gtk_label_set_text(GTK_LABEL(td->TemperatureDegC), result_str);
	
   pFile = fopen ("datalog.txt","a");
   
   entry_cntr++;
   
   fprintf (pFile, "%d %s\n", entry_cntr, reply);
   
   fclose (pFile);
	

  	g_free(result_str);
}


//*****************************************************************
// set_ip_address
//
// Set the ip address value once it is changed
// 
// \param Pointer to the widget that is holding the data
// \param Temperature data
// 
// \return None
//
//****************************************************************
void set_ip_address( GtkWidget *entry, TemperatureData *td)
{
  const gchar *str;
    
  // 
  // 1. Get the text entry from the widget 
  // 2. Set the TemperatureDegF to the string.
  //
  str = gtk_entry_get_text(GTK_ENTRY(entry));
  td->ip_address = str; 
}

//*****************************************************************
// set_ip_address
//
// Set the ip address value once it is changed
// 
// \param Pointer to the widget that is holding the data
// \param Temperature data
// 
// \return None
//
//****************************************************************
void set_repeat_delay( GtkWidget *entry, TemperatureData *td)
{
  const gchar *str;
    
  // 
  // 1. Get the text entry from the widget 
  // 2. Set the TemperatureDegF to the string.
  //
  str = gtk_entry_get_text(GTK_ENTRY(entry));
  td->repeat_delay = atoi( str );
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
  GtkWidget *vbox, *hbox;
  //GtkWidget *entry;
  GtkWidget *frame;
  GtkWidget *convert;
  GtkWidget *ip_label;
  GtkWidget *ip_entry;
 
  TemperatureData td = { .ip_address = NULL, .repeat_delay = 0, .TemperatureDegC = NULL};
  
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

  //------------------
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
  ip_label = gtk_label_new ("MAC Address");
  
  gtk_box_pack_start (GTK_BOX (hbox), ip_label, FALSE, FALSE, 0);
  
  gtk_widget_show (ip_label);
  
  
  //
  // Create a text entry
  // Set max length  = 3
  // Set a max width = 5
  // Pack it to the hbox
  // Connect signal changed to the settemperature()
  // Show the widget
  // Set a tooltip for this 
  //
  ip_entry = gtk_entry_new();
  
  gtk_entry_set_max_length(GTK_ENTRY(ip_entry),20);
  
  gtk_entry_set_width_chars(GTK_ENTRY(ip_entry),20); 
  
  gtk_box_pack_start (GTK_BOX (hbox), ip_entry, FALSE, FALSE, 0);
  
  gtk_widget_show(ip_entry);
  
  g_signal_connect(G_OBJECT(ip_entry), "changed", G_CALLBACK(set_ip_address), &td);
  
  gtk_widget_set_tooltip_text( ip_entry, "Enter the IP address here.");  

  
  //-------------------
  //
  // Create a hbox 
  // Pack the hbox to the vbox
  // Show it
  // 
  hbox = gtk_hbox_new(FALSE, 5);
  
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
  
  gtk_widget_show (hbox);
  
  
  	
   
  //
  // Create a new message frame 
  // Pack the frame to the vbox
  // Show the frame
  // Create a new label with no text in it
  // Add it to the frame 
  // Show the label
  // Show the window
  //
  frame = gtk_frame_new("CMD reply");

  gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 0);

  gtk_widget_show(frame);

  td.TemperatureDegC = gtk_label_new(NULL);
  
  gtk_container_add(GTK_CONTAINER(frame), td.TemperatureDegC);

  gtk_widget_show(td.TemperatureDegC);

  gtk_widget_show(window);

	//**********  
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
  convert = gtk_button_new_with_label("Set MAC");
    
  gtk_box_pack_start(GTK_BOX(vbox), convert, FALSE, TRUE,0);
  
  gtk_widget_show(convert);

  g_signal_connect(G_OBJECT(convert), "clicked", G_CALLBACK(get_reading), &td);
 
  //g_timeout_add( 10000, get_reading, &td );
  
  //
  // Go to gtk_main() and wait
  // 
  gtk_main();

  return 0;
}
