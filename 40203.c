void setup_new_exec(struct linux_binprm * bprm)
{
	arch_pick_mmap_layout(current->mm);

	/* This is the point of no return */
	current->sas_ss_sp = current->sas_ss_size = 0;

	if (uid_eq(current_euid(), current_uid()) && gid_eq(current_egid(), current_gid()))
		set_dumpable(current->mm, SUID_DUMP_USER);
	else
		set_dumpable(current->mm, suid_dumpable);

	set_task_comm(current, bprm->tcomm);

	/* Set the new mm task size. We have to do that late because it may
	 * depend on TIF_32BIT which is only updated in flush_thread() on
	 * some architectures like powerpc
	 */
	current->mm->task_size = TASK_SIZE;

	/* install the new credentials */
	if (!uid_eq(bprm->cred->uid, current_euid()) ||
	    !gid_eq(bprm->cred->gid, current_egid())) {
		current->pdeath_signal = 0;
	} else {
		would_dump(bprm, bprm->file);
		if (bprm->interp_flags & BINPRM_FLAGS_ENFORCE_NONDUMP)
			set_dumpable(current->mm, suid_dumpable);
	}

	/* An exec changes our domain. We are no longer part of the thread
	   group */

	current->self_exec_id++;
			
	flush_signal_handlers(current, 0);
	do_close_on_exec(current->files);
}
