static int get_filter(void __user *arg, struct sock_filter **p)
{
	struct sock_fprog uprog;
	struct sock_filter *code = NULL;
	int len;

	if (copy_from_user(&uprog, arg, sizeof(uprog)))
		return -EFAULT;

	if (!uprog.len) {
		*p = NULL;
		return 0;
	}

	len = uprog.len * sizeof(struct sock_filter);
	code = memdup_user(uprog.filter, len);
	if (IS_ERR(code))
		return PTR_ERR(code);

	*p = code;
	return uprog.len;
}
