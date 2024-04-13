static unsigned long atalk_sum_partial(const unsigned char *data,
				       int len, unsigned long sum)
{
	/* This ought to be unwrapped neatly. I'll trust gcc for now */
	while (len--) {
		sum += *data++;
		sum = rol16(sum, 1);
	}
	return sum;
}
