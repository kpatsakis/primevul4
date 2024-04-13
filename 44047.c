static int futex_proxy_trylock_atomic(u32 __user *pifutex,
				 struct futex_hash_bucket *hb1,
				 struct futex_hash_bucket *hb2,
				 union futex_key *key1, union futex_key *key2,
				 struct futex_pi_state **ps, int set_waiters)
{
	struct futex_q *top_waiter = NULL;
	u32 curval;
	int ret, vpid;

	if (get_futex_value_locked(&curval, pifutex))
		return -EFAULT;

	/*
	 * Find the top_waiter and determine if there are additional waiters.
	 * If the caller intends to requeue more than 1 waiter to pifutex,
	 * force futex_lock_pi_atomic() to set the FUTEX_WAITERS bit now,
	 * as we have means to handle the possible fault.  If not, don't set
	 * the bit unecessarily as it will force the subsequent unlock to enter
	 * the kernel.
	 */
	top_waiter = futex_top_waiter(hb1, key1);

	/* There are no waiters, nothing for us to do. */
	if (!top_waiter)
		return 0;

	/* Ensure we requeue to the expected futex. */
	if (!match_futex(top_waiter->requeue_pi_key, key2))
		return -EINVAL;

	/*
	 * Try to take the lock for top_waiter.  Set the FUTEX_WAITERS bit in
	 * the contended case or if set_waiters is 1.  The pi_state is returned
	 * in ps in contended cases.
	 */
	vpid = task_pid_vnr(top_waiter->task);
	ret = futex_lock_pi_atomic(pifutex, hb2, key2, ps, top_waiter->task,
				   set_waiters);
	if (ret == 1) {
		requeue_pi_wake_futex(top_waiter, key2, hb2);
		return vpid;
	}
	return ret;
}
