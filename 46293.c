static void msg_rcu_free(struct rcu_head *head)
{
	struct ipc_rcu *p = container_of(head, struct ipc_rcu, rcu);
	struct msg_queue *msq = ipc_rcu_to_struct(p);

	security_msg_queue_free(msq);
	ipc_rcu_free(head);
}
