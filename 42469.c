static int __init init_script_binfmt(void)
{
	register_binfmt(&script_format);
	return 0;
}
