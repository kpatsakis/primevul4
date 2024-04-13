 */
PS_SERIALIZER_ENCODE_FUNC(wddx)
{
	wddx_packet *packet;
	zend_string *str;
	PS_ENCODE_VARS;

	packet = php_wddx_constructor();

	php_wddx_packet_start(packet, NULL, 0);
	php_wddx_add_chunk_static(packet, WDDX_STRUCT_S);

	PS_ENCODE_LOOP(
		php_wddx_serialize_var(packet, struc, key);
	);

	php_wddx_add_chunk_static(packet, WDDX_STRUCT_E);
	php_wddx_packet_end(packet);
	smart_str_0(packet);
	str = zend_string_copy(packet->s);
	php_wddx_destructor(packet);

	return str;