int ipc_rcu_getref(void *ptr)
{
	struct ipc_rcu *p = ((struct ipc_rcu *)ptr) - 1;

	return atomic_inc_not_zero(&p->refcount);
}
