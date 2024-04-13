static int __init init(void)
{
	return crypto_register_alg(&alg);
}
