static void hci_sock_cmsg(struct sock *sk, struct msghdr *msg,
			  struct sk_buff *skb)
{
	__u32 mask = hci_pi(sk)->cmsg_mask;

	if (mask & HCI_CMSG_DIR) {
		int incoming = bt_cb(skb)->incoming;
		put_cmsg(msg, SOL_HCI, HCI_CMSG_DIR, sizeof(incoming),
			 &incoming);
	}

	if (mask & HCI_CMSG_TSTAMP) {
#ifdef CONFIG_COMPAT
		struct compat_timeval ctv;
#endif
		struct timeval tv;
		void *data;
		int len;

		skb_get_timestamp(skb, &tv);

		data = &tv;
		len = sizeof(tv);
#ifdef CONFIG_COMPAT
		if (!COMPAT_USE_64BIT_TIME &&
		    (msg->msg_flags & MSG_CMSG_COMPAT)) {
			ctv.tv_sec = tv.tv_sec;
			ctv.tv_usec = tv.tv_usec;
			data = &ctv;
			len = sizeof(ctv);
		}
#endif

		put_cmsg(msg, SOL_HCI, HCI_CMSG_TSTAMP, len, data);
	}
}
