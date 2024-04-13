static int audit_filter_rules(struct task_struct *tsk,
			      struct audit_krule *rule,
			      struct audit_context *ctx,
			      struct audit_names *name,
			      enum audit_state *state,
			      bool task_creation)
{
	const struct cred *cred;
	int i, need_sid = 1;
	u32 sid;

	cred = rcu_dereference_check(tsk->cred, tsk == current || task_creation);

	for (i = 0; i < rule->field_count; i++) {
		struct audit_field *f = &rule->fields[i];
		struct audit_names *n;
		int result = 0;
		pid_t pid;

		switch (f->type) {
		case AUDIT_PID:
			pid = task_pid_nr(tsk);
			result = audit_comparator(pid, f->op, f->val);
			break;
		case AUDIT_PPID:
			if (ctx) {
				if (!ctx->ppid)
					ctx->ppid = task_ppid_nr(tsk);
				result = audit_comparator(ctx->ppid, f->op, f->val);
			}
			break;
		case AUDIT_EXE:
			result = audit_exe_compare(tsk, rule->exe);
			break;
		case AUDIT_UID:
			result = audit_uid_comparator(cred->uid, f->op, f->uid);
			break;
		case AUDIT_EUID:
			result = audit_uid_comparator(cred->euid, f->op, f->uid);
			break;
		case AUDIT_SUID:
			result = audit_uid_comparator(cred->suid, f->op, f->uid);
			break;
		case AUDIT_FSUID:
			result = audit_uid_comparator(cred->fsuid, f->op, f->uid);
			break;
		case AUDIT_GID:
			result = audit_gid_comparator(cred->gid, f->op, f->gid);
			if (f->op == Audit_equal) {
				if (!result)
					result = in_group_p(f->gid);
			} else if (f->op == Audit_not_equal) {
				if (result)
					result = !in_group_p(f->gid);
			}
			break;
		case AUDIT_EGID:
			result = audit_gid_comparator(cred->egid, f->op, f->gid);
			if (f->op == Audit_equal) {
				if (!result)
					result = in_egroup_p(f->gid);
			} else if (f->op == Audit_not_equal) {
				if (result)
					result = !in_egroup_p(f->gid);
			}
			break;
		case AUDIT_SGID:
			result = audit_gid_comparator(cred->sgid, f->op, f->gid);
			break;
		case AUDIT_FSGID:
			result = audit_gid_comparator(cred->fsgid, f->op, f->gid);
			break;
		case AUDIT_PERS:
			result = audit_comparator(tsk->personality, f->op, f->val);
			break;
		case AUDIT_ARCH:
			if (ctx)
				result = audit_comparator(ctx->arch, f->op, f->val);
			break;

		case AUDIT_EXIT:
			if (ctx && ctx->return_valid)
				result = audit_comparator(ctx->return_code, f->op, f->val);
			break;
		case AUDIT_SUCCESS:
			if (ctx && ctx->return_valid) {
				if (f->val)
					result = audit_comparator(ctx->return_valid, f->op, AUDITSC_SUCCESS);
				else
					result = audit_comparator(ctx->return_valid, f->op, AUDITSC_FAILURE);
			}
			break;
		case AUDIT_DEVMAJOR:
			if (name) {
				if (audit_comparator(MAJOR(name->dev), f->op, f->val) ||
				    audit_comparator(MAJOR(name->rdev), f->op, f->val))
					++result;
			} else if (ctx) {
				list_for_each_entry(n, &ctx->names_list, list) {
					if (audit_comparator(MAJOR(n->dev), f->op, f->val) ||
					    audit_comparator(MAJOR(n->rdev), f->op, f->val)) {
						++result;
						break;
					}
				}
			}
			break;
		case AUDIT_DEVMINOR:
			if (name) {
				if (audit_comparator(MINOR(name->dev), f->op, f->val) ||
				    audit_comparator(MINOR(name->rdev), f->op, f->val))
					++result;
			} else if (ctx) {
				list_for_each_entry(n, &ctx->names_list, list) {
					if (audit_comparator(MINOR(n->dev), f->op, f->val) ||
					    audit_comparator(MINOR(n->rdev), f->op, f->val)) {
						++result;
						break;
					}
				}
			}
			break;
		case AUDIT_INODE:
			if (name)
				result = audit_comparator(name->ino, f->op, f->val);
			else if (ctx) {
				list_for_each_entry(n, &ctx->names_list, list) {
					if (audit_comparator(n->ino, f->op, f->val)) {
						++result;
						break;
					}
				}
			}
			break;
		case AUDIT_OBJ_UID:
			if (name) {
				result = audit_uid_comparator(name->uid, f->op, f->uid);
			} else if (ctx) {
				list_for_each_entry(n, &ctx->names_list, list) {
					if (audit_uid_comparator(n->uid, f->op, f->uid)) {
						++result;
						break;
					}
				}
			}
			break;
		case AUDIT_OBJ_GID:
			if (name) {
				result = audit_gid_comparator(name->gid, f->op, f->gid);
			} else if (ctx) {
				list_for_each_entry(n, &ctx->names_list, list) {
					if (audit_gid_comparator(n->gid, f->op, f->gid)) {
						++result;
						break;
					}
				}
			}
			break;
		case AUDIT_WATCH:
			if (name)
				result = audit_watch_compare(rule->watch, name->ino, name->dev);
			break;
		case AUDIT_DIR:
			if (ctx)
				result = match_tree_refs(ctx, rule->tree);
			break;
		case AUDIT_LOGINUID:
			result = audit_uid_comparator(tsk->loginuid, f->op, f->uid);
			break;
		case AUDIT_LOGINUID_SET:
			result = audit_comparator(audit_loginuid_set(tsk), f->op, f->val);
			break;
		case AUDIT_SUBJ_USER:
		case AUDIT_SUBJ_ROLE:
		case AUDIT_SUBJ_TYPE:
		case AUDIT_SUBJ_SEN:
		case AUDIT_SUBJ_CLR:
			/* NOTE: this may return negative values indicating
			   a temporary error.  We simply treat this as a
			   match for now to avoid losing information that
			   may be wanted.   An error message will also be
			   logged upon error */
			if (f->lsm_rule) {
				if (need_sid) {
					security_task_getsecid(tsk, &sid);
					need_sid = 0;
				}
				result = security_audit_rule_match(sid, f->type,
				                                  f->op,
				                                  f->lsm_rule,
				                                  ctx);
			}
			break;
		case AUDIT_OBJ_USER:
		case AUDIT_OBJ_ROLE:
		case AUDIT_OBJ_TYPE:
		case AUDIT_OBJ_LEV_LOW:
		case AUDIT_OBJ_LEV_HIGH:
			/* The above note for AUDIT_SUBJ_USER...AUDIT_SUBJ_CLR
			   also applies here */
			if (f->lsm_rule) {
				/* Find files that match */
				if (name) {
					result = security_audit_rule_match(
					           name->osid, f->type, f->op,
					           f->lsm_rule, ctx);
				} else if (ctx) {
					list_for_each_entry(n, &ctx->names_list, list) {
						if (security_audit_rule_match(n->osid, f->type,
									      f->op, f->lsm_rule,
									      ctx)) {
							++result;
							break;
						}
					}
				}
				/* Find ipc objects that match */
				if (!ctx || ctx->type != AUDIT_IPC)
					break;
				if (security_audit_rule_match(ctx->ipc.osid,
							      f->type, f->op,
							      f->lsm_rule, ctx))
					++result;
			}
			break;
		case AUDIT_ARG0:
		case AUDIT_ARG1:
		case AUDIT_ARG2:
		case AUDIT_ARG3:
			if (ctx)
				result = audit_comparator(ctx->argv[f->type-AUDIT_ARG0], f->op, f->val);
			break;
		case AUDIT_FILTERKEY:
			/* ignore this field for filtering */
			result = 1;
			break;
		case AUDIT_PERM:
			result = audit_match_perm(ctx, f->val);
			break;
		case AUDIT_FILETYPE:
			result = audit_match_filetype(ctx, f->val);
			break;
		case AUDIT_FIELD_COMPARE:
			result = audit_field_compare(tsk, cred, f, ctx, name);
			break;
		}
		if (!result)
			return 0;
	}

	if (ctx) {
		if (rule->prio <= ctx->prio)
			return 0;
		if (rule->filterkey) {
			kfree(ctx->filterkey);
			ctx->filterkey = kstrdup(rule->filterkey, GFP_ATOMIC);
		}
		ctx->prio = rule->prio;
	}
	switch (rule->action) {
	case AUDIT_NEVER:
		*state = AUDIT_DISABLED;
		break;
	case AUDIT_ALWAYS:
		*state = AUDIT_RECORD_CONTEXT;
		break;
	}
	return 1;
}
