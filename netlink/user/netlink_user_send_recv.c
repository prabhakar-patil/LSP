// reference: 	https://gist.github.com/arunk-s/c897bb9d75a6c98733d6

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h> 
#include <linux/netlink.h>

#define NETLINK_USER	31
#define MAX_PAYLOAD	1024

int main (void)
{
	struct sockaddr_nl dest_addr; // kernel module address
	socklen_t len=0;
	ssize_t bytes = 0;
	struct nlmsghdr *nlh = NULL;
	int sock_fd;


	sock_fd = socket (AF_NETLINK, SOCK_RAW, NETLINK_USER);
	if (sock_fd < 0)
	{
		printf ("Error in opening netlink socket: %s\n", strerror(errno));
		exit (EXIT_FAILURE);
	}	

	// fill destination address for socket, which is netlink kernel
	memset (&dest_addr, 0, sizeof(struct sockaddr_nl));
	dest_addr.nl_family = AF_NETLINK;
	dest_addr.nl_pid = 0; //for linux kernel ??
	dest_addr.nl_groups = 0; // unicast message
	len = sizeof (struct sockaddr);

	if (connect (sock_fd, (const struct sockaddr *)&dest_addr, len) < 0) {
		printf ("Error in connecting socket to dest addr: %s\n", strerror(errno));
		exit (EXIT_FAILURE);
	}
	
	// file data in netlink format to send to kernel, 'struct nlmsghdr'
	nlh = (struct nlmsghdr *) malloc (NLMSG_SPACE (MAX_PAYLOAD));
	memset (nlh, 0, NLMSG_SPACE (MAX_PAYLOAD));
	nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD); // sizeof(struct nlmsghdr) + MAX_PAYLOAD
	nlh->nlmsg_pid = getpid ();
	nlh->nlmsg_flags = 0;
	strcpy (NLMSG_DATA(nlh), "Hello"); // data can be copied max 1024 bytes of size=MAX_PAYLOAD

	printf ("Sending message from pid (%d) to kernel,\"%s\"\n", getpid(), (char*) NLMSG_DATA(nlh));
	bytes = send (sock_fd, (void *)nlh, nlh->nlmsg_len, 0);	
	if (bytes < 0) {
		printf ("Error in send(): %s\n", strerror(errno));
		exit (EXIT_FAILURE);
	}
	printf ("Bytes sent: %ld\n", bytes);
	
	printf ("Waiting for message from kernel\n");

	// clear data, it will be filled by kernel module
	memset (nlh, 0, NLMSG_SPACE (MAX_PAYLOAD));

	// read message from kernel
	bytes =	recv (sock_fd, (void *)nlh, NLMSG_SPACE (MAX_PAYLOAD), 0);
	if (bytes < 0) {
		printf ("Error in recv(): %s\n", strerror(errno));
		exit (EXIT_FAILURE);
	}
	printf ("Bytes received: %ld\n", bytes);

	printf ("Received message payload: %s\n", (char *)NLMSG_DATA(nlh));

	close (sock_fd);
	exit (EXIT_SUCCESS);
}

