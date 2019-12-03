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

struct sockaddr_nl src_addr, dest_addr;
struct nlmsghdr *nlh = NULL;
struct iovec iov;
struct msghdr msg;
int sock_fd;

int main (void)
{
	sock_fd = socket (AF_NETLINK, SOCK_RAW, NETLINK_USER);
	if (sock_fd < 0)
	{
		printf ("Error in opening netlink socket: %s\n", strerror(errno));
		exit (EXIT_FAILURE);
	}	

	memset (&src_addr, 0, sizeof(struct sockaddr_nl));
	memset (&dest_addr, 0, sizeof(struct sockaddr_nl));
	dest_addr.nl_family = AF_NETLINK;
	dest_addr.nl_pid = 0; //for linux kernel ??
	dest_addr.nl_groups = 0; // unicast message
	
	nlh = (struct nlmsghdr *) malloc (NLMSG_SPACE (MAX_PAYLOAD));
	memset (nlh, 0, NLMSG_SPACE (MAX_PAYLOAD));
	nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
	nlh->nlmsg_pid = getpid ();
	nlh->nlmsg_flags = 0;
	strcpy (NLMSG_DATA(nlh), "Hello"); // data can be copied max 1024 bytes of size=MAX_PAYLOAD

	iov.iov_base = (void *)nlh;
	iov.iov_len = nlh->nlmsg_len;

	msg.msg_name = (void *)&dest_addr;
	msg.msg_namelen = sizeof (dest_addr);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	printf ("Sending message from pid (%d) to kernel,\"%s\"\n", getpid(), (char*) NLMSG_DATA(nlh));
	sendmsg (sock_fd, &msg, 0);
	printf ("Waiting for message from kernel\n");

	// read message from kernel
	recvmsg (sock_fd, &msg, 0);
	printf ("Received message payload: %s\n", (char *)NLMSG_DATA(nlh));

	close (sock_fd);
	exit (EXIT_SUCCESS);
}

