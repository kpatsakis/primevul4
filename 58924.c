 static inline char *parse_ip_address(php_stream_xport_param *xparam, int *portno)
{
	return parse_ip_address_ex(xparam->inputs.name, xparam->inputs.namelen, portno, xparam->want_errortext, &xparam->outputs.error_text);
}
