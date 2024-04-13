static inline int get_compat_mq_attr(struct mq_attr *attr,
			const struct compat_mq_attr __user *uattr)
{
	struct compat_mq_attr v;

	if (copy_from_user(&v, uattr, sizeof(*uattr)))
		return -EFAULT;

	memset(attr, 0, sizeof(*attr));
	attr->mq_flags = v.mq_flags;
	attr->mq_maxmsg = v.mq_maxmsg;
	attr->mq_msgsize = v.mq_msgsize;
	attr->mq_curmsgs = v.mq_curmsgs;
	return 0;
}
