static int __init michael_mic_init(void)
{
	return crypto_register_shash(&alg);
}
