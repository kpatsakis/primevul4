static bool type_may_be_null(u32 type)
{
	return type & PTR_MAYBE_NULL;
}