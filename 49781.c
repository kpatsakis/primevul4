static int __init padlock_init(void)
{
	int rc = -ENODEV;
	struct cpuinfo_x86 *c = &cpu_data(0);
	struct shash_alg *sha1;
	struct shash_alg *sha256;

	if (!x86_match_cpu(padlock_sha_ids) || !cpu_has_phe_enabled)
		return -ENODEV;

	/* Register the newly added algorithm module if on *
	* VIA Nano processor, or else just do as before */
	if (c->x86_model < 0x0f) {
		sha1 = &sha1_alg;
		sha256 = &sha256_alg;
	} else {
		sha1 = &sha1_alg_nano;
		sha256 = &sha256_alg_nano;
	}

	rc = crypto_register_shash(sha1);
	if (rc)
		goto out;

	rc = crypto_register_shash(sha256);
	if (rc)
		goto out_unreg1;

	printk(KERN_NOTICE PFX "Using VIA PadLock ACE for SHA1/SHA256 algorithms.\n");

	return 0;

out_unreg1:
	crypto_unregister_shash(sha1);

out:
	printk(KERN_ERR PFX "VIA PadLock SHA1/SHA256 initialization failed.\n");
	return rc;
}
