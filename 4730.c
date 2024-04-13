 */
static void php_wddx_serialize_unset(wddx_packet *packet)
{
	php_wddx_add_chunk_static(packet, WDDX_NULL);