static int __init camellia_sparc64_mod_init(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(algs); i++)
		INIT_LIST_HEAD(&algs[i].cra_list);

	if (sparc64_has_camellia_opcode()) {
		pr_info("Using sparc64 camellia opcodes optimized CAMELLIA implementation\n");
		return crypto_register_algs(algs, ARRAY_SIZE(algs));
	}
	pr_info("sparc64 camellia opcodes not available.\n");
	return -ENODEV;
}
