static int sctp_setsockopt_adaptation_layer(struct sock *sk, char __user *optval,
					  int optlen)
{
	struct sctp_setadaptation adaptation;

	if (optlen != sizeof(struct sctp_setadaptation))
		return -EINVAL;
	if (copy_from_user(&adaptation, optval, optlen))
		return -EFAULT;

	sctp_sk(sk)->adaptation_ind = adaptation.ssb_adaptation_ind;

	return 0;
}
