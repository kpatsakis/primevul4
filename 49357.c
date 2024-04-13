static int __init des3_ede_x86_init(void)
{
	if (!force && is_blacklisted_cpu()) {
		pr_info("des3_ede-x86_64: performance on this CPU would be suboptimal: disabling des3_ede-x86_64.\n");
		return -ENODEV;
	}

	return crypto_register_algs(des3_ede_algs, ARRAY_SIZE(des3_ede_algs));
}
