static int trusted_instantiate(struct key *key,
			       struct key_preparsed_payload *prep)
{
	struct trusted_key_payload *payload = NULL;
	struct trusted_key_options *options = NULL;
	size_t datalen = prep->datalen;
	char *datablob;
	int ret = 0;
	int key_cmd;
	size_t key_len;
	int tpm2;

	tpm2 = tpm_is_tpm2(TPM_ANY_NUM);
	if (tpm2 < 0)
		return tpm2;

	if (datalen <= 0 || datalen > 32767 || !prep->data)
		return -EINVAL;

	datablob = kmalloc(datalen + 1, GFP_KERNEL);
	if (!datablob)
		return -ENOMEM;
	memcpy(datablob, prep->data, datalen);
	datablob[datalen] = '\0';

	options = trusted_options_alloc();
	if (!options) {
		ret = -ENOMEM;
		goto out;
	}
	payload = trusted_payload_alloc(key);
	if (!payload) {
		ret = -ENOMEM;
		goto out;
	}

	key_cmd = datablob_parse(datablob, payload, options);
	if (key_cmd < 0) {
		ret = key_cmd;
		goto out;
	}

	if (!options->keyhandle) {
		ret = -EINVAL;
		goto out;
	}

	dump_payload(payload);
	dump_options(options);

	switch (key_cmd) {
	case Opt_load:
		if (tpm2)
			ret = tpm_unseal_trusted(TPM_ANY_NUM, payload, options);
		else
			ret = key_unseal(payload, options);
		dump_payload(payload);
		dump_options(options);
		if (ret < 0)
			pr_info("trusted_key: key_unseal failed (%d)\n", ret);
		break;
	case Opt_new:
		key_len = payload->key_len;
		ret = tpm_get_random(TPM_ANY_NUM, payload->key, key_len);
		if (ret != key_len) {
			pr_info("trusted_key: key_create failed (%d)\n", ret);
			goto out;
		}
		if (tpm2)
			ret = tpm_seal_trusted(TPM_ANY_NUM, payload, options);
		else
			ret = key_seal(payload, options);
		if (ret < 0)
			pr_info("trusted_key: key_seal failed (%d)\n", ret);
		break;
	default:
		ret = -EINVAL;
		goto out;
	}
	if (!ret && options->pcrlock)
		ret = pcrlock(options->pcrlock);
out:
	kfree(datablob);
	kfree(options);
	if (!ret)
		rcu_assign_keypointer(key, payload);
	else
		kfree(payload);
	return ret;
}
