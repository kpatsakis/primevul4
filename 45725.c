static int routing_ioctl(struct net *net, struct socket *sock,
			 unsigned int cmd, void __user *argp)
{
	int ret;
	void *r = NULL;
	struct in6_rtmsg r6;
	struct rtentry r4;
	char devname[16];
	u32 rtdev;
	mm_segment_t old_fs = get_fs();

	if (sock && sock->sk && sock->sk->sk_family == AF_INET6) { /* ipv6 */
		struct in6_rtmsg32 __user *ur6 = argp;
		ret = copy_from_user(&r6.rtmsg_dst, &(ur6->rtmsg_dst),
			3 * sizeof(struct in6_addr));
		ret |= get_user(r6.rtmsg_type, &(ur6->rtmsg_type));
		ret |= get_user(r6.rtmsg_dst_len, &(ur6->rtmsg_dst_len));
		ret |= get_user(r6.rtmsg_src_len, &(ur6->rtmsg_src_len));
		ret |= get_user(r6.rtmsg_metric, &(ur6->rtmsg_metric));
		ret |= get_user(r6.rtmsg_info, &(ur6->rtmsg_info));
		ret |= get_user(r6.rtmsg_flags, &(ur6->rtmsg_flags));
		ret |= get_user(r6.rtmsg_ifindex, &(ur6->rtmsg_ifindex));

		r = (void *) &r6;
	} else { /* ipv4 */
		struct rtentry32 __user *ur4 = argp;
		ret = copy_from_user(&r4.rt_dst, &(ur4->rt_dst),
					3 * sizeof(struct sockaddr));
		ret |= get_user(r4.rt_flags, &(ur4->rt_flags));
		ret |= get_user(r4.rt_metric, &(ur4->rt_metric));
		ret |= get_user(r4.rt_mtu, &(ur4->rt_mtu));
		ret |= get_user(r4.rt_window, &(ur4->rt_window));
		ret |= get_user(r4.rt_irtt, &(ur4->rt_irtt));
		ret |= get_user(rtdev, &(ur4->rt_dev));
		if (rtdev) {
			ret |= copy_from_user(devname, compat_ptr(rtdev), 15);
			r4.rt_dev = (char __user __force *)devname;
			devname[15] = 0;
		} else
			r4.rt_dev = NULL;

		r = (void *) &r4;
	}

	if (ret) {
		ret = -EFAULT;
		goto out;
	}

	set_fs(KERNEL_DS);
	ret = sock_do_ioctl(net, sock, cmd, (unsigned long) r);
	set_fs(old_fs);

out:
	return ret;
}
