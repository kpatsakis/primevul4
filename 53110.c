static int __init evm_display_config(void)
{
	char **xattrname;

	for (xattrname = evm_config_xattrnames; *xattrname != NULL; xattrname++)
		pr_info("%s\n", *xattrname);
	return 0;
}
