- kernel
	kernel module creating netlink socket communication to receive message from user space
	and send messge back to userspace

	socket created using : netlink_kernel_create() and struct netlink_kernel_cfg
	socket domain: AF_NETLINK
	socket protocol: user defined number (31 in example)

- user
	user application creating socket to send message to kernel moduel and receive message
	from kernel module

	socket created using: socket()
	socket domain: AF_NETLINK
	socket protocol: user defined number (31 in example)
	
	send/receive is demostrated using 3 variants send() and/or recv() family of system call

	netlink_user.c 			: sendmsg()/recvmsg() pair used
	netlink_user_sendto_recvfrom.c 	: sendto()/recvfrom() pair used
	netlink_user_send_recv.c 	: connect() and send()/recv() pair used 
