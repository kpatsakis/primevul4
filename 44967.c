static inline int open_to_namei_flags(int flag)
{
	if ((flag+1) & O_ACCMODE)
		flag++;
	return flag;
}
