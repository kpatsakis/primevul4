alloc_mapi_values (MAPI_Attr* a)
{
    if (a && a->num_values)
    {
	a->values = CHECKED_XCALLOC (MAPI_Value, a->num_values);
	return a->values;
    }
    return NULL;
}
