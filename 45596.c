static void netlink_queue_mmaped_skb(struct sock *sk, struct sk_buff *skb)
{
	struct nl_mmap_hdr *hdr;

	hdr = netlink_mmap_hdr(skb);
	hdr->nm_len	= skb->len;
	hdr->nm_group	= NETLINK_CB(skb).dst_group;
	hdr->nm_pid	= NETLINK_CB(skb).creds.pid;
	hdr->nm_uid	= from_kuid(sk_user_ns(sk), NETLINK_CB(skb).creds.uid);
	hdr->nm_gid	= from_kgid(sk_user_ns(sk), NETLINK_CB(skb).creds.gid);
	netlink_frame_flush_dcache(hdr);
	netlink_set_status(hdr, NL_MMAP_STATUS_VALID);

	NETLINK_CB(skb).flags |= NETLINK_SKB_DELIVERED;
	kfree_skb(skb);
}
