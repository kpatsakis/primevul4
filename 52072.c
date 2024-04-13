void xmlrpc_set_buffer(char *(*func) (char *buffer, int len))
{
	return_if_fail(func != NULL);
	xmlrpc.setbuffer = func;
}
