static inline bool is_near(int offset)
{
	return offset <= 127 && offset >= -128;
}
