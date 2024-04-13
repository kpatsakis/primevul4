SYSCALL_DEFINE6(futex, u32 __user *, uaddr, int, op, u32, val,
		struct timespec __user *, utime, u32 __user *, uaddr2,
		u32, val3)
{
	struct timespec ts;
	ktime_t t, *tp = NULL;
	u32 val2 = 0;
	int cmd = op & FUTEX_CMD_MASK;

	if (utime && (cmd == FUTEX_WAIT || cmd == FUTEX_LOCK_PI ||
		      cmd == FUTEX_WAIT_BITSET ||
		      cmd == FUTEX_WAIT_REQUEUE_PI)) {
		if (copy_from_user(&ts, utime, sizeof(ts)) != 0)
			return -EFAULT;
		if (!timespec_valid(&ts))
			return -EINVAL;

		t = timespec_to_ktime(ts);
		if (cmd == FUTEX_WAIT)
			t = ktime_add_safe(ktime_get(), t);
		tp = &t;
	}
	/*
	 * requeue parameter in 'utime' if cmd == FUTEX_*_REQUEUE_*.
	 * number of waiters to wake in 'utime' if cmd == FUTEX_WAKE_OP.
	 */
	if (cmd == FUTEX_REQUEUE || cmd == FUTEX_CMP_REQUEUE ||
	    cmd == FUTEX_CMP_REQUEUE_PI || cmd == FUTEX_WAKE_OP)
		val2 = (u32) (unsigned long) utime;

	return do_futex(uaddr, op, val, tp, uaddr2, val2, val3);
}
