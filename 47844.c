static int __init init_elf_binfmt(void)
{
	register_binfmt(&elf_format);
	return 0;
}
