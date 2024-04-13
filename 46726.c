static int set_ro(const char *val, struct kernel_param *kp)
{
	return kstrtouint(val, 10, (unsigned int *)&start_readonly);
}
