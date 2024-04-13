static int audit_field_compare(struct task_struct *tsk,
			       const struct cred *cred,
			       struct audit_field *f,
			       struct audit_context *ctx,
			       struct audit_names *name)
{
	switch (f->val) {
	/* process to file object comparisons */
	case AUDIT_COMPARE_UID_TO_OBJ_UID:
		return audit_compare_uid(cred->uid, name, f, ctx);
	case AUDIT_COMPARE_GID_TO_OBJ_GID:
		return audit_compare_gid(cred->gid, name, f, ctx);
	case AUDIT_COMPARE_EUID_TO_OBJ_UID:
		return audit_compare_uid(cred->euid, name, f, ctx);
	case AUDIT_COMPARE_EGID_TO_OBJ_GID:
		return audit_compare_gid(cred->egid, name, f, ctx);
	case AUDIT_COMPARE_AUID_TO_OBJ_UID:
		return audit_compare_uid(tsk->loginuid, name, f, ctx);
	case AUDIT_COMPARE_SUID_TO_OBJ_UID:
		return audit_compare_uid(cred->suid, name, f, ctx);
	case AUDIT_COMPARE_SGID_TO_OBJ_GID:
		return audit_compare_gid(cred->sgid, name, f, ctx);
	case AUDIT_COMPARE_FSUID_TO_OBJ_UID:
		return audit_compare_uid(cred->fsuid, name, f, ctx);
	case AUDIT_COMPARE_FSGID_TO_OBJ_GID:
		return audit_compare_gid(cred->fsgid, name, f, ctx);
	/* uid comparisons */
	case AUDIT_COMPARE_UID_TO_AUID:
		return audit_uid_comparator(cred->uid, f->op, tsk->loginuid);
	case AUDIT_COMPARE_UID_TO_EUID:
		return audit_uid_comparator(cred->uid, f->op, cred->euid);
	case AUDIT_COMPARE_UID_TO_SUID:
		return audit_uid_comparator(cred->uid, f->op, cred->suid);
	case AUDIT_COMPARE_UID_TO_FSUID:
		return audit_uid_comparator(cred->uid, f->op, cred->fsuid);
	/* auid comparisons */
	case AUDIT_COMPARE_AUID_TO_EUID:
		return audit_uid_comparator(tsk->loginuid, f->op, cred->euid);
	case AUDIT_COMPARE_AUID_TO_SUID:
		return audit_uid_comparator(tsk->loginuid, f->op, cred->suid);
	case AUDIT_COMPARE_AUID_TO_FSUID:
		return audit_uid_comparator(tsk->loginuid, f->op, cred->fsuid);
	/* euid comparisons */
	case AUDIT_COMPARE_EUID_TO_SUID:
		return audit_uid_comparator(cred->euid, f->op, cred->suid);
	case AUDIT_COMPARE_EUID_TO_FSUID:
		return audit_uid_comparator(cred->euid, f->op, cred->fsuid);
	/* suid comparisons */
	case AUDIT_COMPARE_SUID_TO_FSUID:
		return audit_uid_comparator(cred->suid, f->op, cred->fsuid);
	/* gid comparisons */
	case AUDIT_COMPARE_GID_TO_EGID:
		return audit_gid_comparator(cred->gid, f->op, cred->egid);
	case AUDIT_COMPARE_GID_TO_SGID:
		return audit_gid_comparator(cred->gid, f->op, cred->sgid);
	case AUDIT_COMPARE_GID_TO_FSGID:
		return audit_gid_comparator(cred->gid, f->op, cred->fsgid);
	/* egid comparisons */
	case AUDIT_COMPARE_EGID_TO_SGID:
		return audit_gid_comparator(cred->egid, f->op, cred->sgid);
	case AUDIT_COMPARE_EGID_TO_FSGID:
		return audit_gid_comparator(cred->egid, f->op, cred->fsgid);
	/* sgid comparison */
	case AUDIT_COMPARE_SGID_TO_FSGID:
		return audit_gid_comparator(cred->sgid, f->op, cred->fsgid);
	default:
		WARN(1, "Missing AUDIT_COMPARE define.  Report as a bug\n");
		return 0;
	}
	return 0;
}
