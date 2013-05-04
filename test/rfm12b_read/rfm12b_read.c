#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

#include "../common/common.h"
#include "../../rfm12b_config.h"
#include "../../rfm12b_ioctl.h"

#define RF12_BUF_LEN	128

static volatile int running;

void sig_handler(int signum)
{
	signal(signum, SIG_IGN);
	running = 0;
}

int main(int argc, char** argv)
{
	int fd, len, i;
	char* devname, buf[128];
	unsigned long pkt_cnt;
	time_t tt;
	
	devname = RF12_TESTS_DEV;
	
	fd = open(RF12_TESTS_DEV, O_RDWR);
	if (fd < 0) {
		printf("\nfailed to open %s: %s.\n\n", devname, strerror(errno));
		return fd;
	} else
		printf(
			"\nsuccessfully opened %s as fd %i, entering read loop...\n\n",
			devname, fd
		);

	fflush(stdout);
	signal(SIGINT, sig_handler);
	signal(SIGTERM, sig_handler);
	
	pkt_cnt = 0;
	running = 1;
	do {		
		len = read(fd, buf, RF12_BUF_LEN);
		
		time (&tt);
		
		if (len > 0) {
			printf("%s", ctime(&tt));
			printf("\t%i bytes read\n\t\t", len);
			
			for (i=0; i<len; i++) {
				printf("%d ", buf[i]);
			}
			printf("\n");
			
			fflush(stdout);	
	
			pkt_cnt++;
		} else if (len < 0) {
			break;
		}		
	} while (running);
	
	print_stats(fd);
	
	close(fd);
	
	printf("\n\n%lu packet(s) received.\n\n", pkt_cnt);
	
	return len;
}