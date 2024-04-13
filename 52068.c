void xmlrpc_process(char *buffer, void *userdata)
{
	int retVal = 0;
	XMLRPCCmd *current = NULL;
	XMLRPCCmd *xml;
	char *tmp;
	int ac;
	char **av = NULL;
	char *name = NULL;

	xmlrpc_error_code = 0;

	if (!buffer)
	{
		xmlrpc_error_code = -1;
		return;
	}

	tmp = xmlrpc_parse(buffer);
	if (tmp)
	{
		name = xmlrpc_method(tmp);
		if (name)
		{
			xml = mowgli_patricia_retrieve(XMLRPCCMD, name);
			if (xml)
			{
				ac = xmlrpc_split_buf(tmp, &av);
				if (xml->func)
				{
					retVal = xml->func(userdata, ac, av);
					if (retVal == XMLRPC_CONT)
					{
						current = xml->next;
						while (current && current->func && retVal == XMLRPC_CONT)
						{
							retVal = current->func(userdata, ac, av);
							current = current->next;
						}
					}
					else
					{	/* we assume that XMLRPC_STOP means the handler has given no output */
						xmlrpc_error_code = -7;
						xmlrpc_generic_error(xmlrpc_error_code, "XMLRPC error: First eligible function returned XMLRPC_STOP");
					}
				}
				else
				{
					xmlrpc_error_code = -6;
					xmlrpc_generic_error(xmlrpc_error_code, "XMLRPC error: Method has no registered function");
				}
			}
			else
			{
				xmlrpc_error_code = -4;
				xmlrpc_generic_error(xmlrpc_error_code, "XMLRPC error: Unknown routine called");
			}
		}
		else
		{
			xmlrpc_error_code = -3;
			xmlrpc_generic_error(xmlrpc_error_code, "XMLRPC error: Missing methodRequest or methodName.");
		}
	}
	else
	{
		xmlrpc_error_code = -2;
		xmlrpc_generic_error(xmlrpc_error_code, "XMLRPC error: Invalid document end at line 1");
	}
	free(av);
	free(tmp);
	free(name);
}
