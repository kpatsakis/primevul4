static int __init crct10dif_intel_mod_init(void)
{
	if (!x86_match_cpu(crct10dif_cpu_id))
		return -ENODEV;

	return crypto_register_shash(&alg);
}
