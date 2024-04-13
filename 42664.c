static int sctp_getsockopt_events(struct sock *sk, int len, char __user *optval,
				  int __user *optlen)
{
	if (len != sizeof(struct sctp_event_subscribe))
		return -EINVAL;
	if (copy_to_user(optval, &sctp_sk(sk)->subscribe, len))
		return -EFAULT;
	return 0;
}
