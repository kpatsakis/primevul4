handle_to_flags(int handle)
{
	if (handle_is_ok(handle, HANDLE_FILE))
		return handles[handle].flags;
	return 0;
}
