static int __init initialize_hashrnd(void)
{
	get_random_bytes(&hashrnd, sizeof(hashrnd));
	return 0;
}
