__sigqueue_alloc(int sig, struct task_struct *t, gfp_t flags, int override_rlimit)
{
	struct sigqueue *q = NULL;
	struct user_struct *user;

	/*
	 * Protect access to @t credentials. This can go away when all
	 * callers hold rcu read lock.
	 */
	rcu_read_lock();
	user = get_uid(__task_cred(t)->user);
	atomic_inc(&user->sigpending);
	rcu_read_unlock();

	if (override_rlimit ||
	    atomic_read(&user->sigpending) <=
			task_rlimit(t, RLIMIT_SIGPENDING)) {
		q = kmem_cache_alloc(sigqueue_cachep, flags);
	} else {
		print_dropped_signal(sig);
	}

	if (unlikely(q == NULL)) {
		atomic_dec(&user->sigpending);
		free_uid(user);
	} else {
		INIT_LIST_HEAD(&q->list);
		q->flags = 0;
		q->user = user;
	}

	return q;
}
