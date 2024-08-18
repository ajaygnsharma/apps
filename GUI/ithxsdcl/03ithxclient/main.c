/*  Make the necessary includes and set up the variables.  */

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main( int argc, char *argv[] )
{
    int sockfd[ 2 ];
    int len;
    struct sockaddr_in address;
    int result;
    char cmd[ 10 ]; // = "*SRH\r\n";
    char reply[ 20 ];
    
    if( argc < 1 )
    {
    	printf(" invalid syntax\n" );
    }
    
    if( !strcmp( argv[ 1 ], "-t" ) )
    {
      strcpy( cmd, "*SRT\r\n" ); 	
    }
    else if( !strcmp( argv[ 1 ], "-h" ) )
    {
      strcpy( cmd, "*SRH\r\n" );  	
    }
    else if( !strcmp( argv[ 1 ], "-d" ) )
    {
      strcpy( cmd, "*SRD\r\n" );
    } 
    
    strcpy( reply, "" );

/*  Create a socket for the client.  */
    sockfd[ 0 ] = socket( AF_INET, SOCK_STREAM, 0 );
    
/*  Name the socket, as agreed with the server.  */
    address.sin_family = AF_INET;
    if( !strcmp( argv[ 2 ], "-ip" ) && ( argv[ 3 ] != NULL ) )
    {
      address.sin_addr.s_addr = inet_addr( argv[ 3 ] );
    }
    else
    {
      address.sin_addr.s_addr = inet_addr( "65.241.1.154" );
    }
      
    address.sin_port = htons( 2000 );
    len = sizeof(address);

    /*  Now connect our socket to the server's socket.  */
    result = connect( sockfd[ 0 ], ( struct sockaddr * )&address, len );

    if(result == -1) {
        perror("oops: client3");
        exit(1);
    }

    /*  We can now read/write via sockfd.  */
    write(sockfd[ 0 ], cmd, sizeof( cmd ) );
    read(sockfd[ 0 ], reply, 6);
    printf("%s\n", reply);
    
    
    close(sockfd[ 0 ]);
    exit(0);
}
