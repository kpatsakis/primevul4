static void usbip_dump_buffer(char *buff, int bufflen)
{
	print_hex_dump(KERN_DEBUG, "usbip-core", DUMP_PREFIX_OFFSET, 16, 4,
		       buff, bufflen, false);
}
