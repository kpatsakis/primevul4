lookup_pi_state(u32 uval, struct futex_hash_bucket *hb,
		union futex_key *key, struct futex_pi_state **ps,
		struct task_struct *task)
{
	struct futex_pi_state *pi_state = NULL;
	struct futex_q *this, *next;
	struct task_struct *p;
	pid_t pid = uval & FUTEX_TID_MASK;

	plist_for_each_entry_safe(this, next, &hb->chain, list) {
		if (match_futex(&this->key, key)) {
			/*
			 * Another waiter already exists - bump up
			 * the refcount and return its pi_state:
			 */
			pi_state = this->pi_state;
			/*
			 * Userspace might have messed up non-PI and PI futexes
			 */
			if (unlikely(!pi_state))
				return -EINVAL;

			WARN_ON(!atomic_read(&pi_state->refcount));

			/*
			 * When pi_state->owner is NULL then the owner died
			 * and another waiter is on the fly. pi_state->owner
			 * is fixed up by the task which acquires
			 * pi_state->rt_mutex.
			 *
			 * We do not check for pid == 0 which can happen when
			 * the owner died and robust_list_exit() cleared the
			 * TID.
			 */
			if (pid && pi_state->owner) {
				/*
				 * Bail out if user space manipulated the
				 * futex value.
				 */
				if (pid != task_pid_vnr(pi_state->owner))
					return -EINVAL;
			}

			/*
			 * Protect against a corrupted uval. If uval
			 * is 0x80000000 then pid is 0 and the waiter
			 * bit is set. So the deadlock check in the
			 * calling code has failed and we did not fall
			 * into the check above due to !pid.
			 */
			if (task && pi_state->owner == task)
				return -EDEADLK;

			atomic_inc(&pi_state->refcount);
			*ps = pi_state;

			return 0;
		}
	}

	/*
	 * We are the first waiter - try to look up the real owner and attach
	 * the new pi_state to it, but bail out when TID = 0
	 */
	if (!pid)
		return -ESRCH;
	p = futex_find_get_task(pid);
	if (!p)
		return -ESRCH;

	if (!p->mm) {
		put_task_struct(p);
		return -EPERM;
	}

	/*
	 * We need to look at the task state flags to figure out,
	 * whether the task is exiting. To protect against the do_exit
	 * change of the task flags, we do this protected by
	 * p->pi_lock:
	 */
	raw_spin_lock_irq(&p->pi_lock);
	if (unlikely(p->flags & PF_EXITING)) {
		/*
		 * The task is on the way out. When PF_EXITPIDONE is
		 * set, we know that the task has finished the
		 * cleanup:
		 */
		int ret = (p->flags & PF_EXITPIDONE) ? -ESRCH : -EAGAIN;

		raw_spin_unlock_irq(&p->pi_lock);
		put_task_struct(p);
		return ret;
	}

	pi_state = alloc_pi_state();

	/*
	 * Initialize the pi_mutex in locked state and make 'p'
	 * the owner of it:
	 */
	rt_mutex_init_proxy_locked(&pi_state->pi_mutex, p);

	/* Store the key for possible exit cleanups: */
	pi_state->key = *key;

	WARN_ON(!list_empty(&pi_state->list));
	list_add(&pi_state->list, &p->pi_state_list);
	pi_state->owner = p;
	raw_spin_unlock_irq(&p->pi_lock);

	put_task_struct(p);

	*ps = pi_state;

	return 0;
}
