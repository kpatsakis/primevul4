void ipc_rcu_putref(void *ptr, void (*func)(struct rcu_head *head))
{
	struct ipc_rcu *p = ((struct ipc_rcu *)ptr) - 1;

	if (!atomic_dec_and_test(&p->refcount))
		return;

	call_rcu(&p->rcu, func);
}
