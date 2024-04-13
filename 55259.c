static int sctp_setsockopt_events(struct sock *sk, char __user *optval,
				  unsigned int optlen)
{
	struct sctp_association *asoc;
	struct sctp_ulpevent *event;

	if (optlen > sizeof(struct sctp_event_subscribe))
		return -EINVAL;
	if (copy_from_user(&sctp_sk(sk)->subscribe, optval, optlen))
		return -EFAULT;

	/* At the time when a user app subscribes to SCTP_SENDER_DRY_EVENT,
	 * if there is no data to be sent or retransmit, the stack will
	 * immediately send up this notification.
	 */
	if (sctp_ulpevent_type_enabled(SCTP_SENDER_DRY_EVENT,
				       &sctp_sk(sk)->subscribe)) {
		asoc = sctp_id2assoc(sk, 0);

		if (asoc && sctp_outq_is_empty(&asoc->outqueue)) {
			event = sctp_ulpevent_make_sender_dry_event(asoc,
					GFP_ATOMIC);
			if (!event)
				return -ENOMEM;

			sctp_ulpq_tail_event(&asoc->ulpq, event);
		}
	}

	return 0;
}
