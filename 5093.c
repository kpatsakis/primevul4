static void setup_buffer(uint8_t *buf, unsigned int seed, int len)
{
	int i;
	srandom(seed);
	for (i=0;i<len;i++) buf[i] = random();
}