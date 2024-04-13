static int ahash_algs_register_all(struct hash_device_data *device_data)
{
	int ret;
	int i;
	int count;

	for (i = 0; i < ARRAY_SIZE(hash_algs); i++) {
		ret = crypto_register_ahash(&hash_algs[i].hash);
		if (ret) {
			count = i;
			dev_err(device_data->dev, "%s: alg registration failed\n",
				hash_algs[i].hash.halg.base.cra_driver_name);
			goto unreg;
		}
	}
	return 0;
unreg:
	for (i = 0; i < count; i++)
		crypto_unregister_ahash(&hash_algs[i].hash);
	return ret;
}
