int CAPSTONE_API cs_winkernel_vsnprintf(char *buffer, size_t count, const char *format, va_list argptr)
{
	int result = _vsnprintf(buffer, count, format, argptr);

	if (result == -1 || (size_t)result == count) {
		buffer[count - 1] = '\0';
	}

	if (result == -1) {
		char* tmp = cs_winkernel_malloc(0x1000);
		if (!tmp) {
			return result;
		}

		result = _vsnprintf(tmp, 0x1000, format, argptr);
		NT_ASSERT(result != -1);
		cs_winkernel_free(tmp);
	}

	return result;
}
