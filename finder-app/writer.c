#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <syslog.h>
#include <string.h>

#define NUM_ARGS 2

int main(int argc, char *argv[]) {
    
    int fd;
    ssize_t bwr;
    char* wfn;
    char* wstr;
    
    // Setup syslog logging for the utility using the LOG_USER facility.
    openlog(NULL, 0, LOG_USER);

    if(argc == 3) {

        wfn = argv[1];
        wstr = argv[2];

    } else {
	// Use the syslog capability to log unexpected errors with LOG_ERR level.
        syslog(LOG_ERR, "ERROR: Incorrect number of args (given: %d / required %d)", NUM_ARGS, argc - 1);
        return 1;
    }
    
    fd = creat(wfn, 0644);

    if(fd == -1) {
	// Use the syslog capability to log unexpected errors with LOG_ERR level.
        syslog(LOG_ERR, "ERROR: %m: %s", wfn);
        return 1;
    }

    long int wsize = strlen(wstr);
    bwr = write(fd, wstr, (size_t) wsize);

    if(bwr == -1) {
	// Use the syslog capability to log unexpected errors with LOG_ERR level.
        syslog(LOG_ERR, "ERROR: %m");
        return 1;
    } else if (bwr != wsize) {
	// Use the syslog capability to log unexpected errors with LOG_ERR level.
        syslog(LOG_ERR,"ERROR: Byte Requested: %ld / Bytes Written: %ld",wsize,bwr);
        return 1;
    }

    // Use the syslog capability to write a message “Writing <string> to <file>” 
    // where <string> is the text string written to file (second argument) and <file> is the file created by the script.
    // Written with LOG_DEBUG level.
    syslog(LOG_DEBUG,"Writing %s to %s", wstr, wfn);
    close(fd);
    return 0;

}