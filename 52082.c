void key_invalidate(struct key *key)
{
	kenter("%d", key_serial(key));

	key_check(key);

	if (!test_bit(KEY_FLAG_INVALIDATED, &key->flags)) {
		down_write_nested(&key->sem, 1);
		if (!test_and_set_bit(KEY_FLAG_INVALIDATED, &key->flags))
			key_schedule_gc_links();
		up_write(&key->sem);
	}
}
