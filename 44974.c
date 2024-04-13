void putname(const char *name)
{
	if (unlikely(!audit_dummy_context()))
		audit_putname(name);
	else
		__putname(name);
}
