static const char *fault_name(unsigned int esr)
{
	const struct fault_info *inf = fault_info + (esr & 63);
	return inf->name;
}
