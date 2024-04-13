static int __init log_buf_len_setup(char *str)
{
	unsigned size = memparse(str, &str);

	if (size)
		size = roundup_pow_of_two(size);
	if (size > log_buf_len)
		new_log_buf_len = size;

	return 0;
}
