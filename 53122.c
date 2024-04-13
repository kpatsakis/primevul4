static int __init evm_set_fixmode(char *str)
{
	if (strncmp(str, "fix", 3) == 0)
		evm_fixmode = 1;
	return 0;
}
