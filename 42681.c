SCTP_STATIC int sctp_recvmsg(struct kiocb *iocb, struct sock *sk,
			     struct msghdr *msg, size_t len, int noblock,
			     int flags, int *addr_len)
{
	struct sctp_ulpevent *event = NULL;
	struct sctp_sock *sp = sctp_sk(sk);
	struct sk_buff *skb;
	int copied;
	int err = 0;
	int skb_len;

	SCTP_DEBUG_PRINTK("sctp_recvmsg(%s: %p, %s: %p, %s: %zd, %s: %d, %s: "
			  "0x%x, %s: %p)\n", "sk", sk, "msghdr", msg,
			  "len", len, "knoblauch", noblock,
			  "flags", flags, "addr_len", addr_len);

	sctp_lock_sock(sk);

	if (sctp_style(sk, TCP) && !sctp_sstate(sk, ESTABLISHED)) {
		err = -ENOTCONN;
		goto out;
	}

	skb = sctp_skb_recv_datagram(sk, flags, noblock, &err);
	if (!skb)
		goto out;

	/* Get the total length of the skb including any skb's in the
	 * frag_list.
	 */
	skb_len = skb->len;

	copied = skb_len;
	if (copied > len)
		copied = len;

	err = skb_copy_datagram_iovec(skb, 0, msg->msg_iov, copied);

	event = sctp_skb2event(skb);

	if (err)
		goto out_free;

	sock_recv_timestamp(msg, sk, skb);
	if (sctp_ulpevent_is_notification(event)) {
		msg->msg_flags |= MSG_NOTIFICATION;
		sp->pf->event_msgname(event, msg->msg_name, addr_len);
	} else {
		sp->pf->skb_msgname(skb, msg->msg_name, addr_len);
	}

	/* Check if we allow SCTP_SNDRCVINFO. */
	if (sp->subscribe.sctp_data_io_event)
		sctp_ulpevent_read_sndrcvinfo(event, msg);
#if 0
	/* FIXME: we should be calling IP/IPv6 layers.  */
	if (sk->sk_protinfo.af_inet.cmsg_flags)
		ip_cmsg_recv(msg, skb);
#endif

	err = copied;

	/* If skb's length exceeds the user's buffer, update the skb and
	 * push it back to the receive_queue so that the next call to
	 * recvmsg() will return the remaining data. Don't set MSG_EOR.
	 */
	if (skb_len > copied) {
		msg->msg_flags &= ~MSG_EOR;
		if (flags & MSG_PEEK)
			goto out_free;
		sctp_skb_pull(skb, copied);
		skb_queue_head(&sk->sk_receive_queue, skb);

		/* When only partial message is copied to the user, increase
		 * rwnd by that amount. If all the data in the skb is read,
		 * rwnd is updated when the event is freed.
		 */
		sctp_assoc_rwnd_increase(event->asoc, copied);
		goto out;
	} else if ((event->msg_flags & MSG_NOTIFICATION) ||
		   (event->msg_flags & MSG_EOR))
		msg->msg_flags |= MSG_EOR;
	else
		msg->msg_flags &= ~MSG_EOR;

out_free:
	if (flags & MSG_PEEK) {
		/* Release the skb reference acquired after peeking the skb in
		 * sctp_skb_recv_datagram().
		 */
		kfree_skb(skb);
	} else {
		/* Free the event which includes releasing the reference to
		 * the owner of the skb, freeing the skb and updating the
		 * rwnd.
		 */
		sctp_ulpevent_free(event);
	}
out:
	sctp_release_sock(sk);
	return err;
}
