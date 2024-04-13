int xmlrpc_unregister_method(const char *method)
{
	return_val_if_fail(method != NULL, XMLRPC_ERR_PARAMS);

	mowgli_patricia_delete(XMLRPCCMD, method);

	return XMLRPC_ERR_OK;
}
