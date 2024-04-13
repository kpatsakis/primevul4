static int key_unseal(struct trusted_key_payload *p,
		      struct trusted_key_options *o)
{
	struct tpm_buf *tb;
	int ret;

	tb = kzalloc(sizeof *tb, GFP_KERNEL);
	if (!tb)
		return -ENOMEM;

	ret = tpm_unseal(tb, o->keyhandle, o->keyauth, p->blob, p->blob_len,
			 o->blobauth, p->key, &p->key_len);
	if (ret < 0)
		pr_info("trusted_key: srkunseal failed (%d)\n", ret);
	else
		/* pull migratable flag out of sealed key */
		p->migratable = p->key[--p->key_len];

	kfree(tb);
	return ret;
}
