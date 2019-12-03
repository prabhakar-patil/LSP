// taken from https://gist.github.com/arunk-s/c897bb9d75a6c98733d6

#include <linux/module.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>

#define NETLINK_USER 31

struct sock *nl_sk = NULL;

static void hello_nl_recv_msg (struct sk_buff *skb)
{
	struct nlmsghdr *nlh;
	int pid;
	struct sk_buff *skb_out;
	int msg_size;
	char *msg = "Hello from kernel";
	int res;

	printk (KERN_INFO "Entering: %s\n", __FUNCTION__);

	nlh = (struct nlmsghdr*)skb->data;
	printk (KERN_INFO "Netlink: received msg from process(%d) payload: %s\n",
			nlh->nlmsg_pid, (char*) nlmsg_data(nlh));

	pid = nlh->nlmsg_pid; // pid of sending process
	msg_size = strlen (msg);
	skb_out = nlmsg_new (msg_size, 0);

	if (!skb_out)	
	{
		printk (KERN_ERR "Failed to allocate new skb\n"); 
		return;
	}

	nlh = nlmsg_put (skb_out, 0, 0, NLMSG_DONE, msg_size, 0);
	if (!nlh)
	{
		printk (KERN_ERR "Failed to construct new netlink messag\n");
		return;
	}

	NETLINK_CB (skb_out).dst_group = 0; // not in multicast group
	strncpy (nlmsg_data(nlh), msg, msg_size);
	res = nlmsg_unicast (nl_sk, skb_out, pid);
	if (res < 0)
	{
		printk (KERN_ERR"Error while sending back to user\n");	
		return;
	}
}

static int __init netlink_init (void)
{
	struct netlink_kernel_cfg cfg = {
		.input = hello_nl_recv_msg,
	};

	printk (KERN_INFO"Entering: %s\n", __FUNCTION__);

	nl_sk = netlink_kernel_create (&init_net, NETLINK_USER, &cfg);
	if (!nl_sk)
	{
		printk (KERN_ALERT"Error creating socket\n");
		return -10;
	}
	return 0;
}

static void __exit netlink_exit (void)
{
	printk (KERN_INFO"Exiting netlink module\n");
	netlink_kernel_release (nl_sk);
}


module_init (netlink_init);
module_exit (netlink_exit);

MODULE_LICENSE ("GPL");
