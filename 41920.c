static int __init console_suspend_disable(char *str)
{
	console_suspend_enabled = 0;
	return 1;
}
