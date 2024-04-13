static int get_ro(char *buffer, struct kernel_param *kp)
{
	return sprintf(buffer, "%d", start_readonly);
}
