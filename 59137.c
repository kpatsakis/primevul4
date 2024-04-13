mapi_attr_dump (MAPI_Attr* attr)
{
    char *name = get_mapi_name_str (attr->name);
    char *type = get_mapi_type_str (attr->type);
    size_t i;

    fprintf (stdout, "(MAPI) %s [type: %s] [num_values = %lu] = \n",
	     name, type, (unsigned long)attr->num_values);
    if (attr->guid)
    {
	fprintf (stdout, "\tGUID: ");
	write_guid (stdout, attr->guid);
	fputc ('\n', stdout);
    }
	
    for (i = 0; i < attr->num_names; i++)
      fprintf (stdout, "\tname #%d: '%s'\n", (int)i, attr->names[i].data);

    for (i = 0; i < attr->num_values; i++)
    {
	fprintf (stdout, "\t#%lu [len: %lu] = ",
		 (unsigned long)i,
		 (unsigned long)attr->values[i].len);

	switch (attr->type)
	{
	case szMAPI_NULL:
	    fprintf (stdout, "NULL");
	    break;

	case szMAPI_SHORT:
	    write_int16 (stdout, (int16)attr->values[i].data.bytes2);
	    break;

	case szMAPI_INT:
	    write_int32 (stdout, (int32)attr->values[i].data.bytes4);
	    break;

	case szMAPI_FLOAT:
	case szMAPI_DOUBLE:
	    write_float (stdout, (float)attr->values[i].data.bytes4);
	    break;

	case szMAPI_BOOLEAN:
	    write_boolean (stdout, attr->values[i].data.bytes4);
	    break;

	case szMAPI_STRING:
	case szMAPI_UNICODE_STRING:
	    write_string (stdout, (char*)attr->values[i].data.buf);
	    break;

	case szMAPI_SYSTIME:
	case szMAPI_CURRENCY:
	case szMAPI_INT8BYTE:
	case szMAPI_APPTIME:
	    write_uint64 (stdout, attr->values[i].data.bytes8);
	    break;

	case szMAPI_ERROR:
	    write_uint32 (stdout, attr->values[i].data.bytes4);
	    break;

	case szMAPI_CLSID:
	    write_guid (stdout, &attr->values[i].data.guid);
	    break;

	case szMAPI_OBJECT:
	case szMAPI_BINARY:
	{
	    size_t x;

	    for (x = 0; x < attr->values[i].len; x++)
	    {
		write_byte (stdout, (uint8)attr->values[i].data.buf[x]);
		fputc (' ', stdout);
	    }
	}
	break;

	default:
	    fprintf (stdout, "<unknown type>");
	    break;
	}
	fprintf (stdout, "\n");
    }

    fflush( NULL );
}
