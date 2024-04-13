static int __init floppy_module_init(void)
{
	if (floppy)
		parse_floppy_cfg_string(floppy);
	return floppy_init();
}
