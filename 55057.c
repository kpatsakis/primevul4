static void flag_free_kv(HtKv *kv) {
	free (kv->key);
	free (kv);
}
