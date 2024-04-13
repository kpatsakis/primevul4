static void ahash_algs_unregister_all(struct hash_device_data *device_data)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(hash_algs); i++)
		crypto_unregister_ahash(&hash_algs[i].hash);
}
