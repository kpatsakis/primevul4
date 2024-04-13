static void ip_cmsg_recv_fragsize(struct msghdr *msg, struct sk_buff *skb)
{
	int val;

	if (IPCB(skb)->frag_max_size == 0)
		return;

	val = IPCB(skb)->frag_max_size;
	put_cmsg(msg, SOL_IP, IP_RECVFRAGSIZE, sizeof(val), &val);
}
