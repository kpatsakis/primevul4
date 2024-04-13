static void asymmetric_key_destroy(struct key *key)
{
	struct asymmetric_key_subtype *subtype = asymmetric_key_subtype(key);
	if (subtype) {
		subtype->destroy(key->payload.data);
		module_put(subtype->owner);
		key->type_data.p[0] = NULL;
	}
	kfree(key->type_data.p[1]);
	key->type_data.p[1] = NULL;
}
