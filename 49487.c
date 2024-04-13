static void hexdump(char *note, unsigned char *buf, unsigned int len)
{
	if (dbg) {
		printk(KERN_CRIT "%s", note);
		print_hex_dump(KERN_CONT, "", DUMP_PREFIX_OFFSET,
				16, 1,
				buf, len, false);
	}
}
