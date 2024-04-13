 */
wddx_packet *php_wddx_constructor(void)
{
	smart_str *packet;

	packet = ecalloc(1, sizeof(smart_str));

	return packet;