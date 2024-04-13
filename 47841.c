static void fill_siginfo_note(struct memelfnote *note, user_siginfo_t *csigdata,
		const siginfo_t *siginfo)
{
	mm_segment_t old_fs = get_fs();
	set_fs(KERNEL_DS);
	copy_siginfo_to_user((user_siginfo_t __user *) csigdata, siginfo);
	set_fs(old_fs);
	fill_note(note, "CORE", NT_SIGINFO, sizeof(*csigdata), csigdata);
}
