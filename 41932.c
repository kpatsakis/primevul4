void log_buf_kexec_setup(void)
{
	VMCOREINFO_SYMBOL(log_buf);
	VMCOREINFO_SYMBOL(log_end);
	VMCOREINFO_SYMBOL(log_buf_len);
	VMCOREINFO_SYMBOL(logged_chars);
}
