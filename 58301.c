static unsigned zero_mmap_capabilities(struct file *file)
{
	return NOMMU_MAP_COPY;
}
