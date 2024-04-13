 */
static void release_wddx_packet_rsrc(zend_resource *rsrc)
{
	smart_str *str = (smart_str *)rsrc->ptr;
	smart_str_free(str);
	efree(str);