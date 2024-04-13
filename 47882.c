static bool is_processor_line(const char *line)
{
	int cpu;

	if (sscanf(line, "processor       : %d", &cpu) == 1)
		return true;
	return false;
}
