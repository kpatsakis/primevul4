mapi_attr_free_list (MAPI_Attr** attrs)
{
    int i;
    for (i = 0; attrs && attrs[i]; i++)
    {
	mapi_attr_free (attrs[i]);
	XFREE (attrs[i]);
    }
}
