static int sctp_setsockopt_events(struct sock *sk, char __user *optval,
					int optlen)
{
	if (optlen != sizeof(struct sctp_event_subscribe))
		return -EINVAL;
	if (copy_from_user(&sctp_sk(sk)->subscribe, optval, optlen))
		return -EFAULT;
	return 0;
}
