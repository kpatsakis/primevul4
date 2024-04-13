key_ref_t search_my_process_keyrings(struct key_type *type,
				     const void *description,
				     key_match_func_t match,
				     bool no_state_check,
				     const struct cred *cred)
{
	key_ref_t key_ref, ret, err;

	/* we want to return -EAGAIN or -ENOKEY if any of the keyrings were
	 * searchable, but we failed to find a key or we found a negative key;
	 * otherwise we want to return a sample error (probably -EACCES) if
	 * none of the keyrings were searchable
	 *
	 * in terms of priority: success > -ENOKEY > -EAGAIN > other error
	 */
	key_ref = NULL;
	ret = NULL;
	err = ERR_PTR(-EAGAIN);

	/* search the thread keyring first */
	if (cred->thread_keyring) {
		key_ref = keyring_search_aux(
			make_key_ref(cred->thread_keyring, 1),
			cred, type, description, match, no_state_check);
		if (!IS_ERR(key_ref))
			goto found;

		switch (PTR_ERR(key_ref)) {
		case -EAGAIN: /* no key */
		case -ENOKEY: /* negative key */
			ret = key_ref;
			break;
		default:
			err = key_ref;
			break;
		}
	}

	/* search the process keyring second */
	if (cred->process_keyring) {
		key_ref = keyring_search_aux(
			make_key_ref(cred->process_keyring, 1),
			cred, type, description, match, no_state_check);
		if (!IS_ERR(key_ref))
			goto found;

		switch (PTR_ERR(key_ref)) {
		case -EAGAIN: /* no key */
			if (ret)
				break;
		case -ENOKEY: /* negative key */
			ret = key_ref;
			break;
		default:
			err = key_ref;
			break;
		}
	}

	/* search the session keyring */
	if (cred->session_keyring) {
		rcu_read_lock();
		key_ref = keyring_search_aux(
			make_key_ref(rcu_dereference(cred->session_keyring), 1),
			cred, type, description, match, no_state_check);
		rcu_read_unlock();

		if (!IS_ERR(key_ref))
			goto found;

		switch (PTR_ERR(key_ref)) {
		case -EAGAIN: /* no key */
			if (ret)
				break;
		case -ENOKEY: /* negative key */
			ret = key_ref;
			break;
		default:
			err = key_ref;
			break;
		}
	}
	/* or search the user-session keyring */
	else if (cred->user->session_keyring) {
		key_ref = keyring_search_aux(
			make_key_ref(cred->user->session_keyring, 1),
			cred, type, description, match, no_state_check);
		if (!IS_ERR(key_ref))
			goto found;

		switch (PTR_ERR(key_ref)) {
		case -EAGAIN: /* no key */
			if (ret)
				break;
		case -ENOKEY: /* negative key */
			ret = key_ref;
			break;
		default:
			err = key_ref;
			break;
		}
	}

	/* no key - decide on the error we're going to go for */
	key_ref = ret ? ret : err;

found:
	return key_ref;
}
