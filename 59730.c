cifs_spnego_key_destroy(struct key *key)
{
	kfree(key->payload.data);
}
