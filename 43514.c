static bool parse_extranonce(struct pool *pool, json_t *val)
{
	char *nonce1;
	int n2size;

	nonce1 = json_array_string(val, 0);
	if (!nonce1) {
		return false;
	}
	n2size = json_integer_value(json_array_get(val, 1));
	if (!n2size) {
		free(nonce1);
		return false;
	}

	cg_wlock(&pool->data_lock);
	pool->nonce1 = nonce1;
	pool->n1_len = strlen(nonce1) / 2;
	free(pool->nonce1bin);
	pool->nonce1bin = (unsigned char *)calloc(pool->n1_len, 1);
	if (unlikely(!pool->nonce1bin))
		quithere(1, "Failed to calloc pool->nonce1bin");
	hex2bin(pool->nonce1bin, pool->nonce1, pool->n1_len);
	pool->n2size = n2size;
	cg_wunlock(&pool->data_lock);

	applog(LOG_NOTICE, "%s extranonce change requested", get_pool_name(pool));

	return true;
}
