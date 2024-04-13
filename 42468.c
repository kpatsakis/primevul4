static void __exit exit_script_binfmt(void)
{
	unregister_binfmt(&script_format);
}
