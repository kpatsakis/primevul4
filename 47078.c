static int iov_num_pages(void __user *iov_base, size_t iov_len)
{
	return (PAGE_ALIGN((unsigned long)iov_base + iov_len) -
	       ((unsigned long)iov_base & PAGE_MASK)) >> PAGE_SHIFT;
}
