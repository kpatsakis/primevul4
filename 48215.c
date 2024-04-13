int __init crypto_fpu_init(void)
{
	return crypto_register_template(&crypto_fpu_tmpl);
}
