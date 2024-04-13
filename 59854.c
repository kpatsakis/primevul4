void user_revoke(struct key *key)
{
	struct user_key_payload *upayload = key->payload.data;

	/* clear the quota */
	key_payload_reserve(key, 0);

	if (upayload) {
		rcu_assign_keypointer(key, NULL);
		kfree_rcu(upayload, rcu);
	}
}
