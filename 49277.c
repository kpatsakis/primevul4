static int __init init(void)
{
	if (!force && is_blacklisted_cpu()) {
		printk(KERN_INFO
			"camellia-x86_64: performance on this CPU "
			"would be suboptimal: disabling "
			"camellia-x86_64.\n");
		return -ENODEV;
	}

	return crypto_register_algs(camellia_algs, ARRAY_SIZE(camellia_algs));
}
