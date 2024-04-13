void key_set_timeout(struct key *key, unsigned timeout)
{
	struct timespec now;
	time_t expiry = 0;

	/* make the changes with the locks held to prevent races */
	down_write(&key->sem);

	if (timeout > 0) {
		now = current_kernel_time();
		expiry = now.tv_sec + timeout;
	}

	key->expiry = expiry;
	key_schedule_gc(key->expiry + key_gc_delay);

	up_write(&key->sem);
}
