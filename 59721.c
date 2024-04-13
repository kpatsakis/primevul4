static void asymmetric_key_free_preparse(struct key_preparsed_payload *prep)
{
	struct asymmetric_key_subtype *subtype = prep->type_data[0];

	pr_devel("==>%s()\n", __func__);

	if (subtype) {
		subtype->destroy(prep->payload[0]);
		module_put(subtype->owner);
	}
	kfree(prep->type_data[1]);
	kfree(prep->description);
}
