static int __init init(void)
{
	if (!force && is_blacklisted_cpu()) {
		printk(KERN_INFO
			"blowfish-x86_64: performance on this CPU "
			"would be suboptimal: disabling "
			"blowfish-x86_64.\n");
		return -ENODEV;
	}

	return crypto_register_algs(bf_algs, ARRAY_SIZE(bf_algs));
}
