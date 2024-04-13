struct key *key_lookup(key_serial_t id)
{
	struct rb_node *n;
	struct key *key;

	spin_lock(&key_serial_lock);

	/* search the tree for the specified key */
	n = key_serial_tree.rb_node;
	while (n) {
		key = rb_entry(n, struct key, serial_node);

		if (id < key->serial)
			n = n->rb_left;
		else if (id > key->serial)
			n = n->rb_right;
		else
			goto found;
	}

not_found:
	key = ERR_PTR(-ENOKEY);
	goto error;

found:
	/* pretend it doesn't exist if it is awaiting deletion */
	if (atomic_read(&key->usage) == 0)
		goto not_found;

	/* this races with key_put(), but that doesn't matter since key_put()
	 * doesn't actually change the key
	 */
	__key_get(key);

error:
	spin_unlock(&key_serial_lock);
	return key;
}
