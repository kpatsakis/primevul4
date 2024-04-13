void __audit_seccomp(unsigned long syscall, long signr, int code)
{
	struct audit_buffer *ab;

	ab = audit_log_start(NULL, GFP_KERNEL, AUDIT_SECCOMP);
	if (unlikely(!ab))
		return;
	audit_log_task(ab);
	audit_log_format(ab, " sig=%ld arch=%x syscall=%ld compat=%d ip=0x%lx code=0x%x",
			 signr, syscall_get_arch(), syscall,
			 in_compat_syscall(), KSTK_EIP(current), code);
	audit_log_end(ab);
}
