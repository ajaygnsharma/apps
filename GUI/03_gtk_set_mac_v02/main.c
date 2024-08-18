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
#define MAXLINE 30

//*******************************************************************
// 
// sendMACAddr
//
// Send the MAC Address to the device
//
// \param Widget Pointer 
// \param Data Pointer
// 
// \return none
//
//******************************************************************
void sendMACAddr(GtkWidget *entry, MACWidgetData *md)
{
  	gchar *result_str;

  	int sockfd;
  	int len;
  	struct sockaddr_in address;
  	int result;
	char str[50];// = "s -MC00:03:34:00:b8:6f\r\n";
  	char reply[ MAXLINE + 1 ];
  	FILE * pFile;
    int n;
  	
	/*  Create a socket for the client.  */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

	/*  Name the socket, as agreed with the server.  */
    address.sin_family = AF_INET;
    //address.sin_addr.s_addr = inet_addr("192.168.1.200");
    address.sin_addr.s_addr = inet_addr( md->ipAddr );
    address.sin_port = htons( 2002 );
    len = sizeof(address);

	/*  Now connect our socket to the server's socket.  */
    result = connect(sockfd, (struct sockaddr *)&address, len);

    if( result == -1 ) 
    {
        //perror("oops: client3");
        //exit(1);
        result_str = ( gchar * )malloc( 100 ); 
    	strcpy( result_str, "CANNOT CONNECT" );
		gtk_label_set_text( GTK_LABEL( md->MACWidget ), result_str );
    }
    else
    {
    	pFile = fopen ("datalog.txt","w");
   
    	/*  We can now read/write via sockfd.  */
    	n = read( sockfd, reply, MAXLINE );//0.01\r\n
		reply[ n ] = 0;
		fprintf( pFile, "%s\n", reply);	
	
		n = read( sockfd, reply, MAXLINE );//iTHX-SD>\r\n
		reply[ n ] = 0;
		fprintf( pFile, "%s\n", reply);	
	
		strcpy( str, "s -MC" );
		strcat( str, md->macAddr ); 	
		strcat( str, "\r\n" );
		write( sockfd, str, strlen( str ) );
		n = read(sockfd, reply, MAXLINE );//CMD_OK\r\niTHX-SD>\r\n
		reply[ n ] = 0;
		fprintf( pFile, "%s\n", reply); 	
	
		write( sockfd, "c\r\n", sizeof( "c\r\n" ) );
		n = read(sockfd, reply, MAXLINE );//SAVE_OK\r\niTHX-SD>\r\n
		reply[ n ] = 0;
		fprintf( pFile, "%s\n", reply);
		
		result_str = (gchar *)malloc(100); 
    	strcpy( result_str, "DONE" );
		gtk_label_set_text(GTK_LABEL(md->MACWidget), result_str);
   		
   		fclose (pFile);
    }
    
    close(sockfd);
    	
  	g_free(result_str);
}


//*****************************************************************
// setMACAddr
//
// Set the MAC address value once it is changed
// 
// \param Pointer to the widget that is holding the data
// \param Temperature data
// 
// \return None
//
//****************************************************************
void setMACAddr( GtkWidget *entry, MACWidgetData *md)
{
  const gchar *str;
    
  // 
  // 1. Get the text entry from the widget 
  // 2. Set the MAC address to the string.
  //
  str = gtk_entry_get_text( GTK_ENTRY( entry ) );
  md->macAddr = str; 
}

//*****************************************************************
// setIPAddr
//
// Set the IP address value once it is changed
// 
// \param Pointer to the widget that is holding the data
// \param Temperature data
// 
// \return None
//
//****************************************************************
void setIPAddr( GtkWidget *entry, MACWidgetData *md)
{
  const gchar *str;
    
  // 
  // 1. Get the text entry from the widget 
  // 2. Set the MAC address to the string.
  //
  str = gtk_entry_get_text( GTK_ENTRY( entry ) );
  md->ipAddr = str; 
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
  	GtkWidget *sendMAC;
  	GtkWidget *ipLabel;
  	GtkWidget *ipEntry;
	GtkWidget *macLabel;
  	GtkWidget *macEntry;
 
  	MACWidgetData md = { .ipAddr = NULL, .macAddr = 0, .MACWidget = NULL};
  
  	//
  	// Init the GTK components 
  	// Create the top level Window
  	// Center the widget
  	//
  	gtk_init(&argc, &argv);
  	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  	gtk_window_set_default_size(GTK_WINDOW(window), 230, 150);
  	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_widget_show(window);

  	//
  	// Create the function gtk_main_quit to quit the application when the 
  	// [x] button is clicked. and set title
  	//
  	g_signal_connect(G_OBJECT(window), "delete-event", G_CALLBACK(gtk_main_quit), NULL);
  	gtk_window_set_title(GTK_WINDOW(window),"MAC Conf");

  
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
  	ipLabel = gtk_label_new ("IP Address");
  	gtk_box_pack_start (GTK_BOX (hbox), ipLabel, FALSE, FALSE, 0);
  	gtk_widget_show (ipLabel);
  
  
  	//
  	// Create a text entry
  	// Set max length  = 3
  	// Set a max width = 5
  	// Pack it to the hbox
  	// Connect signal changed to the settemperature()
  	// Show the widget
  	// Set a tooltip for this 
  	//
 	ipEntry = gtk_entry_new();
  	gtk_entry_set_max_length(GTK_ENTRY(ipEntry),15);
  	gtk_entry_set_width_chars(GTK_ENTRY(ipEntry),15); 
  	gtk_box_pack_start (GTK_BOX (hbox), ipEntry, FALSE, FALSE, 0);
  	gtk_widget_show(ipEntry);
  	
  	g_signal_connect(G_OBJECT( ipEntry ), "changed", G_CALLBACK(setIPAddr), &md);
  	gtk_widget_set_tooltip_text( ipEntry, "Enter the IP address here.");  


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
  	macLabel = gtk_label_new ("MAC Address");
  	gtk_box_pack_start (GTK_BOX (hbox), macLabel, FALSE, FALSE, 0);
  	gtk_widget_show (macLabel);
  
  
  	//
  	// Create a text entry
  	// Set max length  = 3
  	// Set a max width = 5
  	// Pack it to the hbox
  	// Connect signal changed to the settemperature()
  	// Show the widget
  	// Set a tooltip for this 
  	//
 	macEntry = gtk_entry_new();
  	gtk_entry_set_max_length(GTK_ENTRY(macEntry),20);
  	gtk_entry_set_width_chars(GTK_ENTRY(macEntry),20); 
  	gtk_box_pack_start (GTK_BOX (hbox), macEntry, FALSE, FALSE, 0);
  	gtk_widget_show(macEntry);
  	
  	g_signal_connect(G_OBJECT(macEntry), "changed", G_CALLBACK(setMACAddr), &md);
  	gtk_widget_set_tooltip_text( macEntry, "Enter the MAC address here.");  

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

	md.MACWidget = gtk_label_new(NULL);
	gtk_container_add(GTK_CONTAINER(frame), md.MACWidget);
  	gtk_widget_show(md.MACWidget);
 	//gtk_widget_show(window);

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
  	sendMAC = gtk_button_new_with_label("Set MAC");
    gtk_box_pack_start(GTK_BOX(vbox), sendMAC, FALSE, TRUE,0);
  	gtk_widget_show(sendMAC);
  	g_signal_connect( G_OBJECT(sendMAC), "clicked", G_CALLBACK(sendMACAddr), &md );
 
  	//
  	// Go to gtk_main() and wait
  	// 
  	gtk_main();

  	return 0;
}
