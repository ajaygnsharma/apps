#include <stdio.h>
#include <string.h>

void strsort( int (const char *, const char *), char ** );
int lengthcmp( const char *, const char * );

int main( int argc, char **argv)
{
   char **tmp;

   tmp = argv;
   strsort( strcmp, argv );
   while ( *tmp != NULL )
      printf("\t%s\n", *tmp++ );
   printf("\n");
   tmp = argv;
   strsort( lengthcmp, argv );
   while ( *tmp != NULL )
      printf("\t%s\n", *tmp++ );
   return 0;
}

void strsort( int cmp(const char *, const char *), char **strings )
{
   char *temp, **ptr;
   int i, j, length = 0;

   for( ptr = strings; *ptr != NULL; ptr++ ) length++;
   for( i = 0; i < length - 1; ++i )
      for( j = length - 1; j > i; --j )
         if( cmp( *(strings + i), *(strings + j) ) > 0 ) {
            temp = *(strings + i);
            *(strings + i) = *(strings + j);
            *(strings + j) = temp;
         }
}

int lengthcmp( const char *s1, const char *s2 )
{
   int l1, l2;
   l1 = strlen( s1 );
   l2 = strlen( s2 );
   if( l1 < l2 ) return -1;
   if( l1 == l2 ) return 0;
   return 1;
}