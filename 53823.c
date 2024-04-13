long keyctl_assume_authority(key_serial_t id)
{
	struct key *authkey;
	long ret;

	/* special key IDs aren't permitted */
	ret = -EINVAL;
	if (id < 0)
		goto error;

	/* we divest ourselves of authority if given an ID of 0 */
	if (id == 0) {
		ret = keyctl_change_reqkey_auth(NULL);
		goto error;
	}

	/* attempt to assume the authority temporarily granted to us whilst we
	 * instantiate the specified key
	 * - the authorisation key must be in the current task's keyrings
	 *   somewhere
	 */
	authkey = key_get_instantiation_authkey(id);
	if (IS_ERR(authkey)) {
		ret = PTR_ERR(authkey);
		goto error;
	}

	ret = keyctl_change_reqkey_auth(authkey);
	if (ret < 0)
		goto error;
	key_put(authkey);

	ret = authkey->serial;
error:
	return ret;
}
