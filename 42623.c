int config_error_nonbool(const char *var)
{
	return error("Missing value for '%s'", var);
}
