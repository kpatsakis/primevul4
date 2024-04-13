asmlinkage long sys_oabi_socketcall(int call, unsigned long __user *args)
{
	unsigned long r = -EFAULT, a[6];

	switch (call) {
	case SYS_BIND:
		if (copy_from_user(a, args, 3 * sizeof(long)) == 0)
			r = sys_oabi_bind(a[0], (struct sockaddr __user *)a[1], a[2]);
		break;
	case SYS_CONNECT:
		if (copy_from_user(a, args, 3 * sizeof(long)) == 0)
			r = sys_oabi_connect(a[0], (struct sockaddr __user *)a[1], a[2]);
		break;
	case SYS_SENDTO:
		if (copy_from_user(a, args, 6 * sizeof(long)) == 0)
			r = sys_oabi_sendto(a[0], (void __user *)a[1], a[2], a[3],
					    (struct sockaddr __user *)a[4], a[5]);
		break;
	case SYS_SENDMSG:
		if (copy_from_user(a, args, 3 * sizeof(long)) == 0)
			r = sys_oabi_sendmsg(a[0], (struct user_msghdr __user *)a[1], a[2]);
		break;
	default:
		r = sys_socketcall(call, args);
	}

	return r;
}
