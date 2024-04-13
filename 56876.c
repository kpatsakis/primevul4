static inline int put_compat_mq_attr(const struct mq_attr *attr,
			struct compat_mq_attr __user *uattr)
{
	struct compat_mq_attr v;

	memset(&v, 0, sizeof(v));
	v.mq_flags = attr->mq_flags;
	v.mq_maxmsg = attr->mq_maxmsg;
	v.mq_msgsize = attr->mq_msgsize;
	v.mq_curmsgs = attr->mq_curmsgs;
	if (copy_to_user(uattr, &v, sizeof(*uattr)))
		return -EFAULT;
	return 0;
}
