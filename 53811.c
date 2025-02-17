static void trusted_rcu_free(struct rcu_head *rcu)
{
	struct trusted_key_payload *p;

	p = container_of(rcu, struct trusted_key_payload, rcu);
	memset(p->key, 0, p->key_len);
	kfree(p);
}
