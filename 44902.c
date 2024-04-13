static int futex_lock_pi_atomic(u32 __user *uaddr, struct futex_hash_bucket *hb,
				union futex_key *key,
				struct futex_pi_state **ps,
				struct task_struct *task, int set_waiters)
{
	int lock_taken, ret, ownerdied = 0;
	u32 uval, newval, curval;

retry:
	ret = lock_taken = 0;

	/*
	 * To avoid races, we attempt to take the lock here again
	 * (by doing a 0 -> TID atomic cmpxchg), while holding all
	 * the locks. It will most likely not succeed.
	 */
	newval = task_pid_vnr(task);
	if (set_waiters)
		newval |= FUTEX_WAITERS;

	curval = cmpxchg_futex_value_locked(uaddr, 0, newval);

	if (unlikely(curval == -EFAULT))
		return -EFAULT;

	/*
	 * Detect deadlocks.
	 */
	if ((unlikely((curval & FUTEX_TID_MASK) == task_pid_vnr(task))))
		return -EDEADLK;

	/*
	 * Surprise - we got the lock. Just return to userspace:
	 */
	if (unlikely(!curval))
		return 1;

	uval = curval;

	/*
	 * Set the FUTEX_WAITERS flag, so the owner will know it has someone
	 * to wake at the next unlock.
	 */
	newval = curval | FUTEX_WAITERS;

	/*
	 * There are two cases, where a futex might have no owner (the
	 * owner TID is 0): OWNER_DIED. We take over the futex in this
	 * case. We also do an unconditional take over, when the owner
	 * of the futex died.
	 *
	 * This is safe as we are protected by the hash bucket lock !
	 */
	if (unlikely(ownerdied || !(curval & FUTEX_TID_MASK))) {
		/* Keep the OWNER_DIED bit */
		newval = (curval & ~FUTEX_TID_MASK) | task_pid_vnr(task);
		ownerdied = 0;
		lock_taken = 1;
	}

	curval = cmpxchg_futex_value_locked(uaddr, uval, newval);

	if (unlikely(curval == -EFAULT))
		return -EFAULT;
	if (unlikely(curval != uval))
		goto retry;

	/*
	 * We took the lock due to owner died take over.
	 */
	if (unlikely(lock_taken))
		return 1;

	/*
	 * We dont have the lock. Look up the PI state (or create it if
	 * we are the first waiter):
	 */
	ret = lookup_pi_state(uval, hb, key, ps);

	if (unlikely(ret)) {
		switch (ret) {
		case -ESRCH:
			/*
			 * No owner found for this futex. Check if the
			 * OWNER_DIED bit is set to figure out whether
			 * this is a robust futex or not.
			 */
			if (get_futex_value_locked(&curval, uaddr))
				return -EFAULT;

			/*
			 * We simply start over in case of a robust
			 * futex. The code above will take the futex
			 * and return happy.
			 */
			if (curval & FUTEX_OWNER_DIED) {
				ownerdied = 1;
				goto retry;
			}
		default:
			break;
		}
	}

	return ret;
}
