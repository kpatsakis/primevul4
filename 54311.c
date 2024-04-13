struct tty_struct *alloc_tty_struct(void)
{
	return kzalloc(sizeof(struct tty_struct), GFP_KERNEL);
}
