static long get_nr_dentry_unused(void)
{
	int i;
	long sum = 0;
	for_each_possible_cpu(i)
		sum += per_cpu(nr_dentry_unused, i);
	return sum < 0 ? 0 : sum;
}
