static long futex_wait_restart(struct restart_block *restart)
{
	u32 __user *uaddr = (u32 __user *)restart->futex.uaddr;
	int fshared = 0;
	ktime_t t, *tp = NULL;

	if (restart->futex.flags & FLAGS_HAS_TIMEOUT) {
		t.tv64 = restart->futex.time;
		tp = &t;
	}
	restart->fn = do_no_restart_syscall;
	if (restart->futex.flags & FLAGS_SHARED)
		fshared = 1;
	return (long)futex_wait(uaddr, fshared, restart->futex.val, tp,
				restart->futex.bitset,
				restart->futex.flags & FLAGS_CLOCKRT);
}
