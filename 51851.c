static void tipc_sk_callback(struct rcu_head *head)
{
	struct tipc_sock *tsk = container_of(head, struct tipc_sock, rcu);

	sock_put(&tsk->sk);
}
