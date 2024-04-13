static inline int kvmppc_get_vsr_word_offset(int index)
{
	int offset;

	if ((index > 3) || (index < 0))
		return -1;

#ifdef __BIG_ENDIAN
	offset = index;
#else
	offset = 3 - index;
#endif
	return offset;
}
