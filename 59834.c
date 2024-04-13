int generic_key_instantiate(struct key *key, struct key_preparsed_payload *prep)
{
	int ret;

	pr_devel("==>%s()\n", __func__);

	ret = key_payload_reserve(key, prep->quotalen);
	if (ret == 0) {
		key->type_data.p[0] = prep->type_data[0];
		key->type_data.p[1] = prep->type_data[1];
		rcu_assign_keypointer(key, prep->payload[0]);
		key->payload.data2[1] = prep->payload[1];
		prep->type_data[0] = NULL;
		prep->type_data[1] = NULL;
		prep->payload[0] = NULL;
		prep->payload[1] = NULL;
	}
	pr_devel("<==%s() = %d\n", __func__, ret);
	return ret;
}
