static void deferred_put_nlk_sk(struct rcu_head *head)
{
	struct netlink_sock *nlk = container_of(head, struct netlink_sock, rcu);

	sock_put(&nlk->sk);
}
