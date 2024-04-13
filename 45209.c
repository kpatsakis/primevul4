int compat_sock_get_timestamp(struct sock *sk, struct timeval __user *userstamp)
{
	struct compat_timeval __user *ctv;
	int err;
	struct timeval tv;

	if (COMPAT_USE_64BIT_TIME)
		return sock_get_timestamp(sk, userstamp);

	ctv = (struct compat_timeval __user *) userstamp;
	err = -ENOENT;
	if (!sock_flag(sk, SOCK_TIMESTAMP))
		sock_enable_timestamp(sk, SOCK_TIMESTAMP);
	tv = ktime_to_timeval(sk->sk_stamp);
	if (tv.tv_sec == -1)
		return err;
	if (tv.tv_sec == 0) {
		sk->sk_stamp = ktime_get_real();
		tv = ktime_to_timeval(sk->sk_stamp);
	}
	err = 0;
	if (put_user(tv.tv_sec, &ctv->tv_sec) ||
			put_user(tv.tv_usec, &ctv->tv_usec))
		err = -EFAULT;
	return err;
}
