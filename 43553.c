void _recalloc(void **ptr, size_t old, size_t new, const char *file, const char *func, const int line)
{
	if (new == old)
		return;
	*ptr = realloc(*ptr, new);
	if (unlikely(!*ptr))
		quitfrom(1, file, func, line, "Failed to realloc");
	if (new > old)
		memset(*ptr + old, 0, new - old);
}
