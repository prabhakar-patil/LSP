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
	struct sockaddr_nl src_addr, dest_addr;
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
	
	// file data in netlink format to send to kernel, 'struct nlmsghdr'
	nlh = (struct nlmsghdr *) malloc (NLMSG_SPACE (MAX_PAYLOAD));
	memset (nlh, 0, NLMSG_SPACE (MAX_PAYLOAD));
	nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD); // sizeof(struct nlmsghdr) + MAX_PAYLOAD
	nlh->nlmsg_pid = getpid ();
	nlh->nlmsg_flags = 0;
	strcpy (NLMSG_DATA(nlh), "Hello"); // data can be copied max 1024 bytes of size=MAX_PAYLOAD

	printf ("Sending message from pid (%d) to kernel,\"%s\"\n", getpid(), (char*) NLMSG_DATA(nlh));
	sendto (sock_fd, (void *)nlh,
		nlh->nlmsg_len,
		0,
		(const struct sockaddr*)&dest_addr,
		sizeof(dest_addr));	
	
	printf ("Waiting for message from kernel\n");

	// clear src_addr, it will be filled by kernel module, which is source of message
	memset (&src_addr, 0, sizeof(struct sockaddr_nl));
	socklen_t slen = sizeof(struct sockaddr);

	// clear data, it will be filled by kernel module
	memset (nlh, 0, NLMSG_SPACE (MAX_PAYLOAD));

	// read message from kernel
	recvfrom (sock_fd,
		  (void *)nlh,
		  NLMSG_SPACE(MAX_PAYLOAD),
		  0,
		  (struct sockaddr*)&src_addr,
		  &slen);

	printf ("Received message payload: %s\n", (char *)NLMSG_DATA(nlh));

	close (sock_fd);
	exit (EXIT_SUCCESS);
}

