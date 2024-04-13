void iriap_cleanup(void)
{
	irlmp_unregister_service(service_handle);

	hashbin_delete(iriap, (FREE_FUNC) __iriap_close);
	hashbin_delete(irias_objects, (FREE_FUNC) __irias_delete_object);
}
