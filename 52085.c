void key_revoke(struct key *key)
{
	struct timespec now;
	time_t time;

	key_check(key);

	/* make sure no one's trying to change or use the key when we mark it
	 * - we tell lockdep that we might nest because we might be revoking an
	 *   authorisation key whilst holding the sem on a key we've just
	 *   instantiated
	 */
	down_write_nested(&key->sem, 1);
	if (!test_and_set_bit(KEY_FLAG_REVOKED, &key->flags) &&
	    key->type->revoke)
		key->type->revoke(key);

	/* set the death time to no more than the expiry time */
	now = current_kernel_time();
	time = now.tv_sec;
	if (key->revoked_at == 0 || key->revoked_at > time) {
		key->revoked_at = time;
		key_schedule_gc(key->revoked_at + key_gc_delay);
	}

	up_write(&key->sem);
}
