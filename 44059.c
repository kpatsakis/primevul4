int handle_futex_death(u32 __user *uaddr, struct task_struct *curr, int pi)
{
	u32 uval, uninitialized_var(nval), mval;

retry:
	if (get_user(uval, uaddr))
		return -1;

	if ((uval & FUTEX_TID_MASK) == task_pid_vnr(curr)) {
		/*
		 * Ok, this dying thread is truly holding a futex
		 * of interest. Set the OWNER_DIED bit atomically
		 * via cmpxchg, and if the value had FUTEX_WAITERS
		 * set, wake up a waiter (if any). (We have to do a
		 * futex_wake() even if OWNER_DIED is already set -
		 * to handle the rare but possible case of recursive
		 * thread-death.) The rest of the cleanup is done in
		 * userspace.
		 */
		mval = (uval & FUTEX_WAITERS) | FUTEX_OWNER_DIED;
		/*
		 * We are not holding a lock here, but we want to have
		 * the pagefault_disable/enable() protection because
		 * we want to handle the fault gracefully. If the
		 * access fails we try to fault in the futex with R/W
		 * verification via get_user_pages. get_user() above
		 * does not guarantee R/W access. If that fails we
		 * give up and leave the futex locked.
		 */
		if (cmpxchg_futex_value_locked(&nval, uaddr, uval, mval)) {
			if (fault_in_user_writeable(uaddr))
				return -1;
			goto retry;
		}
		if (nval != uval)
			goto retry;

		/*
		 * Wake robust non-PI futexes here. The wakeup of
		 * PI futexes happens in exit_pi_state():
		 */
		if (!pi && (uval & FUTEX_WAITERS))
			futex_wake(uaddr, 1, 1, FUTEX_BITSET_MATCH_ANY);
	}
	return 0;
}
