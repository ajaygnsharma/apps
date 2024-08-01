/*
 * main.c
 *
 *  Created on: Feb 9, 2017
 *      Author: ajay.sharma
 */
/*
 * main.c
 *
 *  Created on: Apr 18, 2012
 *      Author: Ajay Sharma
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <getopt.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/termios.h>
#include <sys/time.h>

char *version = "1.0.0";

struct  __attribute__((__packed__)) plp_cmd_s {
  unsigned char      status;
  unsigned char      substatus;
  unsigned int       length;
  unsigned short int reserved;
  unsigned short int capacitance;
  unsigned short int cap_voltage;
  unsigned short int cap_charge_time;
  unsigned short int init_cap_voltage;
};

/*****************************************************************************
 * usage(): how to use the file */
void usage(FILE *fp, int rc){
    fprintf(fp, "Usage: ipp [-h] [-f filename]"
            "\n\n");
    fprintf(fp, "        -h            this help\n");
    fprintf(fp, "        -v             print version info\n");
    fprintf(fp, "        -f filename    file containing ip addr & password\n");
    exit(rc);
}

/*****************************************************************************
 * main() */
int main(int argc, char *argv[]){
    FILE        *fp;
    int     c;
    char    *ipp_file;
    int n = 0;
    //unsigned char *buf;

    ipp_file = "./foo.bin";

    while ((c = getopt(argc, argv, "vh?f:")) > 0) {
        switch (c) {
        case 'v':
            printf("%s: version %s\n", argv[0], version);
            exit(0);
        case 'f':
            ipp_file = optarg;
            break;
        case 'h':
        case '?':
            usage(stdout, 0);
            break;
        default:
            fprintf(stderr, "ERROR: unkown option '%c'\n", c);
            usage(stderr, 1);
            break;
        }
    }

    /* Check device is real, and open it. */
    if ((fp = fopen(ipp_file, "rb")) == NULL) {
        fprintf(stderr, "ERROR: failed to open %s, errno=%d\n",
                ipp_file, errno);
        exit(0);
    }

    if (setvbuf(fp, NULL, _IONBF, 0) != 0) {
        fprintf(stderr, "ERROR: failed to setvbuf(%s), errno=%d\n",
                ipp_file, errno);
        exit(0);
    }

    struct plp_cmd_s *plp_cmd;
    plp_cmd = malloc(sizeof(struct plp_cmd_s));

    if (fseek(fp, 0, SEEK_SET) < 0)
          return(-1);
    n = fread(plp_cmd,sizeof(struct plp_cmd_s),1,fp);

    fscanf(fp,"%hhu%hhu%x%hu%hu%hu%hu%hu\n",
            &plp_cmd->status,
            &plp_cmd->substatus,
            &plp_cmd->length,
            &plp_cmd->reserved,
            &plp_cmd->capacitance,
            &plp_cmd->cap_voltage,
            &plp_cmd->cap_charge_time,
            &plp_cmd->init_cap_voltage);
    printf("Status=%X\n",plp_cmd->status);
    printf("Sub Status=%X\n",plp_cmd->substatus);
    printf("Length=%X\n",plp_cmd->length);
    printf("Capacitance=%d uF\n",plp_cmd->capacitance);
    printf("Capacitor Voltage=%d V\n",plp_cmd->cap_voltage);
    printf("Capacitor Charge time=%d ms\n",plp_cmd->cap_charge_time);
    printf("Initial capacitor voltage=%d V\n",plp_cmd->cap_voltage);

    free(plp_cmd);

    printf("Read %d bytes\n",n);

    fclose(fp);

    exit(0);
}



