static inline int valid_user_sp(unsigned long sp, int is_64)
{
	if (!sp || (sp & 7) || sp > TASK_SIZE - 32)
		return 0;
	return 1;
}
