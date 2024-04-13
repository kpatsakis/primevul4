static int key_seal(struct trusted_key_payload *p,
		    struct trusted_key_options *o)
{
	struct tpm_buf *tb;
	int ret;

	tb = kzalloc(sizeof *tb, GFP_KERNEL);
	if (!tb)
		return -ENOMEM;

	/* include migratable flag at end of sealed key */
	p->key[p->key_len] = p->migratable;

	ret = tpm_seal(tb, o->keytype, o->keyhandle, o->keyauth,
		       p->key, p->key_len + 1, p->blob, &p->blob_len,
		       o->blobauth, o->pcrinfo, o->pcrinfo_len);
	if (ret < 0)
		pr_info("trusted_key: srkseal failed (%d)\n", ret);

	kfree(tb);
	return ret;
}
