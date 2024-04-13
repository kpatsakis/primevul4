file_add_attr (File* file, Attr* attr)
{
    assert (file && attr);
    if (!(file && attr)) return;

    /* we only care about some things... we will skip most attributes */
    switch (attr->name)
    {
    case attATTACHMODIFYDATE:
	copy_date_from_attr (attr, &file->dt);
	break;

    case attATTACHMENT:
    {
	MAPI_Attr **mapi_attrs = mapi_attr_read (attr->len, attr->buf);
	if (mapi_attrs)
	{
	    file_add_mapi_attrs (file, mapi_attrs);
	    mapi_attr_free_list (mapi_attrs);
	    XFREE (mapi_attrs);
	}
    }
    break;

    case attATTACHTITLE:
	file->name = strdup( (char*)attr->buf );
	break;

    case attATTACHDATA:
	file->len = attr->len;
	file->data = CHECKED_XMALLOC(unsigned char, attr->len);
	memmove (file->data, attr->buf, attr->len);
	break;

    default:
	break;
    }
}
