key_ref_t lookup_user_key(key_serial_t id, unsigned long lflags,
			  key_perm_t perm)
{
	struct request_key_auth *rka;
	const struct cred *cred;
	struct key *key;
	key_ref_t key_ref, skey_ref;
	int ret;

try_again:
	cred = get_current_cred();
	key_ref = ERR_PTR(-ENOKEY);

	switch (id) {
	case KEY_SPEC_THREAD_KEYRING:
		if (!cred->thread_keyring) {
			if (!(lflags & KEY_LOOKUP_CREATE))
				goto error;

			ret = install_thread_keyring();
			if (ret < 0) {
				key_ref = ERR_PTR(ret);
				goto error;
			}
			goto reget_creds;
		}

		key = cred->thread_keyring;
		atomic_inc(&key->usage);
		key_ref = make_key_ref(key, 1);
		break;

	case KEY_SPEC_PROCESS_KEYRING:
		if (!cred->process_keyring) {
			if (!(lflags & KEY_LOOKUP_CREATE))
				goto error;

			ret = install_process_keyring();
			if (ret < 0) {
				key_ref = ERR_PTR(ret);
				goto error;
			}
			goto reget_creds;
		}

		key = cred->process_keyring;
		atomic_inc(&key->usage);
		key_ref = make_key_ref(key, 1);
		break;

	case KEY_SPEC_SESSION_KEYRING:
		if (!cred->session_keyring) {
			/* always install a session keyring upon access if one
			 * doesn't exist yet */
			ret = install_user_keyrings();
			if (ret < 0)
				goto error;
			if (lflags & KEY_LOOKUP_CREATE)
				ret = join_session_keyring(NULL);
			else
				ret = install_session_keyring(
					cred->user->session_keyring);

			if (ret < 0)
				goto error;
			goto reget_creds;
		} else if (cred->session_keyring ==
			   cred->user->session_keyring &&
			   lflags & KEY_LOOKUP_CREATE) {
			ret = join_session_keyring(NULL);
			if (ret < 0)
				goto error;
			goto reget_creds;
		}

		rcu_read_lock();
		key = rcu_dereference(cred->session_keyring);
		atomic_inc(&key->usage);
		rcu_read_unlock();
		key_ref = make_key_ref(key, 1);
		break;

	case KEY_SPEC_USER_KEYRING:
		if (!cred->user->uid_keyring) {
			ret = install_user_keyrings();
			if (ret < 0)
				goto error;
		}

		key = cred->user->uid_keyring;
		atomic_inc(&key->usage);
		key_ref = make_key_ref(key, 1);
		break;

	case KEY_SPEC_USER_SESSION_KEYRING:
		if (!cred->user->session_keyring) {
			ret = install_user_keyrings();
			if (ret < 0)
				goto error;
		}

		key = cred->user->session_keyring;
		atomic_inc(&key->usage);
		key_ref = make_key_ref(key, 1);
		break;

	case KEY_SPEC_GROUP_KEYRING:
		/* group keyrings are not yet supported */
		key_ref = ERR_PTR(-EINVAL);
		goto error;

	case KEY_SPEC_REQKEY_AUTH_KEY:
		key = cred->request_key_auth;
		if (!key)
			goto error;

		atomic_inc(&key->usage);
		key_ref = make_key_ref(key, 1);
		break;

	case KEY_SPEC_REQUESTOR_KEYRING:
		if (!cred->request_key_auth)
			goto error;

		down_read(&cred->request_key_auth->sem);
		if (test_bit(KEY_FLAG_REVOKED,
			     &cred->request_key_auth->flags)) {
			key_ref = ERR_PTR(-EKEYREVOKED);
			key = NULL;
		} else {
			rka = cred->request_key_auth->payload.data;
			key = rka->dest_keyring;
			atomic_inc(&key->usage);
		}
		up_read(&cred->request_key_auth->sem);
		if (!key)
			goto error;
		key_ref = make_key_ref(key, 1);
		break;

	default:
		key_ref = ERR_PTR(-EINVAL);
		if (id < 1)
			goto error;

		key = key_lookup(id);
		if (IS_ERR(key)) {
			key_ref = ERR_CAST(key);
			goto error;
		}

		key_ref = make_key_ref(key, 0);

		/* check to see if we possess the key */
		skey_ref = search_process_keyrings(key->type, key,
						   lookup_user_key_possessed,
						   cred);

		if (!IS_ERR(skey_ref)) {
			key_put(key);
			key_ref = skey_ref;
		}

		break;
	}

	/* unlink does not use the nominated key in any way, so can skip all
	 * the permission checks as it is only concerned with the keyring */
	if (lflags & KEY_LOOKUP_FOR_UNLINK) {
		ret = 0;
		goto error;
	}

	if (!(lflags & KEY_LOOKUP_PARTIAL)) {
		ret = wait_for_key_construction(key, true);
		switch (ret) {
		case -ERESTARTSYS:
			goto invalid_key;
		default:
			if (perm)
				goto invalid_key;
		case 0:
			break;
		}
	} else if (perm) {
		ret = key_validate(key);
		if (ret < 0)
			goto invalid_key;
	}

	ret = -EIO;
	if (!(lflags & KEY_LOOKUP_PARTIAL) &&
	    !test_bit(KEY_FLAG_INSTANTIATED, &key->flags))
		goto invalid_key;

	/* check the permissions */
	ret = key_task_permission(key_ref, cred, perm);
	if (ret < 0)
		goto invalid_key;

	key->last_used_at = current_kernel_time().tv_sec;

error:
	put_cred(cred);
	return key_ref;

invalid_key:
	key_ref_put(key_ref);
	key_ref = ERR_PTR(ret);
	goto error;

	/* if we attempted to install a keyring, then it may have caused new
	 * creds to be installed */
reget_creds:
	put_cred(cred);
	goto try_again;
}
