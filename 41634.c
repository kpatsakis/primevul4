static int sctp_getsockopt_assoc_number(struct sock *sk, int len,
				    char __user *optval, int __user *optlen)
{
	struct sctp_sock *sp = sctp_sk(sk);
	struct sctp_association *asoc;
	u32 val = 0;

	if (sctp_style(sk, TCP))
		return -EOPNOTSUPP;

	if (len < sizeof(u32))
		return -EINVAL;

	len = sizeof(u32);

	list_for_each_entry(asoc, &(sp->ep->asocs), asocs) {
		val++;
	}

	if (put_user(len, optlen))
		return -EFAULT;
	if (copy_to_user(optval, &val, len))
		return -EFAULT;

	return 0;
}
