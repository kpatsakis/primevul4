int evm_init_key(void)
{
	struct key *evm_key;
	struct encrypted_key_payload *ekp;
	int rc = 0;

	evm_key = request_key(&key_type_encrypted, EVMKEY, NULL);
	if (IS_ERR(evm_key))
		return -ENOENT;

	down_read(&evm_key->sem);
	ekp = evm_key->payload.data;
	if (ekp->decrypted_datalen > MAX_KEY_SIZE) {
		rc = -EINVAL;
		goto out;
	}
	memcpy(evmkey, ekp->decrypted_data, ekp->decrypted_datalen);
out:
	/* burn the original key contents */
	memset(ekp->decrypted_data, 0, ekp->decrypted_datalen);
	up_read(&evm_key->sem);
	key_put(evm_key);
	return rc;
}
