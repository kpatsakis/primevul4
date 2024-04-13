wddx_packet *php_wddx_constructor(void)
{
	smart_str *packet;

	packet = (smart_str *)emalloc(sizeof(smart_str));
	packet->c = NULL;

	return packet;
}
