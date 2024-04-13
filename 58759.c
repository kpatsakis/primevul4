static int ipx_recvmsg(struct socket *sock, struct msghdr *msg, size_t size,
		       int flags)
{
	struct sock *sk = sock->sk;
	struct ipx_sock *ipxs = ipx_sk(sk);
	DECLARE_SOCKADDR(struct sockaddr_ipx *, sipx, msg->msg_name);
	struct ipxhdr *ipx = NULL;
	struct sk_buff *skb;
	int copied, rc;
	bool locked = true;

	lock_sock(sk);
	/* put the autobinding in */
	if (!ipxs->port) {
		struct sockaddr_ipx uaddr;

		uaddr.sipx_port		= 0;
		uaddr.sipx_network 	= 0;

#ifdef CONFIG_IPX_INTERN
		rc = -ENETDOWN;
		if (!ipxs->intrfc)
			goto out; /* Someone zonked the iface */
		memcpy(uaddr.sipx_node, ipxs->intrfc->if_node, IPX_NODE_LEN);
#endif	/* CONFIG_IPX_INTERN */

		rc = __ipx_bind(sock, (struct sockaddr *)&uaddr,
			      sizeof(struct sockaddr_ipx));
		if (rc)
			goto out;
	}

	rc = -ENOTCONN;
	if (sock_flag(sk, SOCK_ZAPPED))
		goto out;

	release_sock(sk);
	locked = false;
	skb = skb_recv_datagram(sk, flags & ~MSG_DONTWAIT,
				flags & MSG_DONTWAIT, &rc);
	if (!skb) {
		if (rc == -EAGAIN && (sk->sk_shutdown & RCV_SHUTDOWN))
			rc = 0;
		goto out;
	}

	ipx 	= ipx_hdr(skb);
	copied 	= ntohs(ipx->ipx_pktsize) - sizeof(struct ipxhdr);
	if (copied > size) {
		copied = size;
		msg->msg_flags |= MSG_TRUNC;
	}

	rc = skb_copy_datagram_msg(skb, sizeof(struct ipxhdr), msg, copied);
	if (rc)
		goto out_free;
	if (skb->tstamp)
		sk->sk_stamp = skb->tstamp;

	if (sipx) {
		sipx->sipx_family	= AF_IPX;
		sipx->sipx_port		= ipx->ipx_source.sock;
		memcpy(sipx->sipx_node, ipx->ipx_source.node, IPX_NODE_LEN);
		sipx->sipx_network	= IPX_SKB_CB(skb)->ipx_source_net;
		sipx->sipx_type 	= ipx->ipx_type;
		sipx->sipx_zero		= 0;
		msg->msg_namelen	= sizeof(*sipx);
	}
	rc = copied;

out_free:
	skb_free_datagram(sk, skb);
out:
	if (locked)
		release_sock(sk);
	return rc;
}
