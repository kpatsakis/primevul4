sched_feat_write(struct file *filp, const char __user *ubuf,
		size_t cnt, loff_t *ppos)
{
	char buf[64];
	char *cmp;
	int i;

	if (cnt > 63)
		cnt = 63;

	if (copy_from_user(&buf, ubuf, cnt))
		return -EFAULT;

	buf[cnt] = 0;
	cmp = strstrip(buf);

	i = sched_feat_set(cmp);
	if (i == __SCHED_FEAT_NR)
		return -EINVAL;

	*ppos += cnt;

	return cnt;
}
