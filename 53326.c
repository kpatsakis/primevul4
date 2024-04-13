static void pty_line_name(struct tty_driver *driver, int index, char *p)
{
	int i = index + driver->name_base;
	/* ->name is initialized to "ttyp", but "tty" is expected */
	sprintf(p, "%s%c%x",
		driver->subtype == PTY_TYPE_SLAVE ? "tty" : driver->name,
		ptychar[i >> 4 & 0xf], i & 0xf);
}
