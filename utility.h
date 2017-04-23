#include <signal.h>
#include <sys/time.h>


void signal_bind_wrapper(int signo, void (*func)(int));

void start_timer(int sec, int usec);


int bind_wrapper(int socket_no, char * ipaddress, int port_no, int reusable);