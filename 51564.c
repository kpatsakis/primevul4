static void audit_log_exit(struct audit_context *context, struct task_struct *tsk)
{
	int i, call_panic = 0;
	struct audit_buffer *ab;
	struct audit_aux_data *aux;
	struct audit_names *n;

	/* tsk == current */
	context->personality = tsk->personality;

	ab = audit_log_start(context, GFP_KERNEL, AUDIT_SYSCALL);
	if (!ab)
		return;		/* audit_panic has been called */
	audit_log_format(ab, "arch=%x syscall=%d",
			 context->arch, context->major);
	if (context->personality != PER_LINUX)
		audit_log_format(ab, " per=%lx", context->personality);
	if (context->return_valid)
		audit_log_format(ab, " success=%s exit=%ld",
				 (context->return_valid==AUDITSC_SUCCESS)?"yes":"no",
				 context->return_code);

	audit_log_format(ab,
			 " a0=%lx a1=%lx a2=%lx a3=%lx items=%d",
			 context->argv[0],
			 context->argv[1],
			 context->argv[2],
			 context->argv[3],
			 context->name_count);

	audit_log_task_info(ab, tsk);
	audit_log_key(ab, context->filterkey);
	audit_log_end(ab);

	for (aux = context->aux; aux; aux = aux->next) {

		ab = audit_log_start(context, GFP_KERNEL, aux->type);
		if (!ab)
			continue; /* audit_panic has been called */

		switch (aux->type) {

		case AUDIT_BPRM_FCAPS: {
			struct audit_aux_data_bprm_fcaps *axs = (void *)aux;
			audit_log_format(ab, "fver=%x", axs->fcap_ver);
			audit_log_cap(ab, "fp", &axs->fcap.permitted);
			audit_log_cap(ab, "fi", &axs->fcap.inheritable);
			audit_log_format(ab, " fe=%d", axs->fcap.fE);
			audit_log_cap(ab, "old_pp", &axs->old_pcap.permitted);
			audit_log_cap(ab, "old_pi", &axs->old_pcap.inheritable);
			audit_log_cap(ab, "old_pe", &axs->old_pcap.effective);
			audit_log_cap(ab, "new_pp", &axs->new_pcap.permitted);
			audit_log_cap(ab, "new_pi", &axs->new_pcap.inheritable);
			audit_log_cap(ab, "new_pe", &axs->new_pcap.effective);
			break; }

		}
		audit_log_end(ab);
	}

	if (context->type)
		show_special(context, &call_panic);

	if (context->fds[0] >= 0) {
		ab = audit_log_start(context, GFP_KERNEL, AUDIT_FD_PAIR);
		if (ab) {
			audit_log_format(ab, "fd0=%d fd1=%d",
					context->fds[0], context->fds[1]);
			audit_log_end(ab);
		}
	}

	if (context->sockaddr_len) {
		ab = audit_log_start(context, GFP_KERNEL, AUDIT_SOCKADDR);
		if (ab) {
			audit_log_format(ab, "saddr=");
			audit_log_n_hex(ab, (void *)context->sockaddr,
					context->sockaddr_len);
			audit_log_end(ab);
		}
	}

	for (aux = context->aux_pids; aux; aux = aux->next) {
		struct audit_aux_data_pids *axs = (void *)aux;

		for (i = 0; i < axs->pid_count; i++)
			if (audit_log_pid_context(context, axs->target_pid[i],
						  axs->target_auid[i],
						  axs->target_uid[i],
						  axs->target_sessionid[i],
						  axs->target_sid[i],
						  axs->target_comm[i]))
				call_panic = 1;
	}

	if (context->target_pid &&
	    audit_log_pid_context(context, context->target_pid,
				  context->target_auid, context->target_uid,
				  context->target_sessionid,
				  context->target_sid, context->target_comm))
			call_panic = 1;

	if (context->pwd.dentry && context->pwd.mnt) {
		ab = audit_log_start(context, GFP_KERNEL, AUDIT_CWD);
		if (ab) {
			audit_log_d_path(ab, "cwd=", &context->pwd);
			audit_log_end(ab);
		}
	}

	i = 0;
	list_for_each_entry(n, &context->names_list, list) {
		if (n->hidden)
			continue;
		audit_log_name(context, n, NULL, i++, &call_panic);
	}

	audit_log_proctitle(tsk, context);

	/* Send end of event record to help user space know we are finished */
	ab = audit_log_start(context, GFP_KERNEL, AUDIT_EOE);
	if (ab)
		audit_log_end(ab);
	if (call_panic)
		audit_panic("error converting sid to string");
}
