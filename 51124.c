void php_wddx_destructor(wddx_packet *packet)
{
	smart_str_free(packet);
	efree(packet);
}
