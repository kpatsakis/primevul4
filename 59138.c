mapi_attr_free (MAPI_Attr* attr)
{
    if (attr)
    {
	size_t i;
	for (i = 0; i < attr->num_values; i++)
	{
	    if ((attr->type == szMAPI_STRING)
		|| (attr->type == szMAPI_UNICODE_STRING)
		|| (attr->type == szMAPI_BINARY))
	    {
		XFREE (attr->values[i].data.buf);
	    }
	}
        if (attr->num_names > 0) {
            for (i = 0; i < attr->num_names; i++)
            {
                XFREE(attr->names[i].data);
            }
            XFREE(attr->names);
        }
	XFREE (attr->values);
	XFREE (attr->guid);
	memset (attr, '\0', sizeof (MAPI_Attr));
    }
}
