 */
static void php_wddx_serialize_boolean(wddx_packet *packet, zval *var)
{
	php_wddx_add_chunk(packet, Z_TYPE_P(var) == IS_TRUE ? WDDX_BOOLEAN_TRUE : WDDX_BOOLEAN_FALSE);