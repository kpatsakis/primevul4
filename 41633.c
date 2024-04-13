static int sctp_getsockopt_assoc_ids(struct sock *sk, int len,
				    char __user *optval, int __user *optlen)
{
	struct sctp_sock *sp = sctp_sk(sk);
	struct sctp_association *asoc;
	struct sctp_assoc_ids *ids;
	u32 num = 0;

	if (sctp_style(sk, TCP))
		return -EOPNOTSUPP;

	if (len < sizeof(struct sctp_assoc_ids))
		return -EINVAL;

	list_for_each_entry(asoc, &(sp->ep->asocs), asocs) {
		num++;
	}

	if (len < sizeof(struct sctp_assoc_ids) + sizeof(sctp_assoc_t) * num)
		return -EINVAL;

	len = sizeof(struct sctp_assoc_ids) + sizeof(sctp_assoc_t) * num;

	ids = kmalloc(len, GFP_KERNEL);
	if (unlikely(!ids))
		return -ENOMEM;

	ids->gaids_number_of_ids = num;
	num = 0;
	list_for_each_entry(asoc, &(sp->ep->asocs), asocs) {
		ids->gaids_assoc_id[num++] = asoc->assoc_id;
	}

	if (put_user(len, optlen) || copy_to_user(optval, ids, len)) {
		kfree(ids);
		return -EFAULT;
	}

	kfree(ids);
	return 0;
}
