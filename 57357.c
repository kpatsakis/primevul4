static void _ht_node_free_kv(HtKv *kv) {
	free (kv->key);
	free (kv);
}
