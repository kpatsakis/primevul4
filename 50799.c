static void nfnetlink_rcv(struct sk_buff *skb)
{
	struct nlmsghdr *nlh = nlmsg_hdr(skb);
	u_int16_t res_id;
	int msglen;

	if (nlh->nlmsg_len < NLMSG_HDRLEN ||
	    skb->len < nlh->nlmsg_len)
		return;

	if (!netlink_net_capable(skb, CAP_NET_ADMIN)) {
		netlink_ack(skb, nlh, -EPERM);
		return;
	}

	if (nlh->nlmsg_type == NFNL_MSG_BATCH_BEGIN) {
		struct nfgenmsg *nfgenmsg;

		msglen = NLMSG_ALIGN(nlh->nlmsg_len);
		if (msglen > skb->len)
			msglen = skb->len;

		if (nlh->nlmsg_len < NLMSG_HDRLEN ||
		    skb->len < NLMSG_HDRLEN + sizeof(struct nfgenmsg))
			return;

		nfgenmsg = nlmsg_data(nlh);
		skb_pull(skb, msglen);
		/* Work around old nft using host byte order */
		if (nfgenmsg->res_id == NFNL_SUBSYS_NFTABLES)
			res_id = NFNL_SUBSYS_NFTABLES;
		else
			res_id = ntohs(nfgenmsg->res_id);
		nfnetlink_rcv_batch(skb, nlh, res_id);
	} else {
		netlink_rcv_skb(skb, &nfnetlink_rcv_msg);
	}
}
