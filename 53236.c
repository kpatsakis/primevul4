static int nfs4_acl_nfsv4_to_posix(struct nfs4_acl *acl,
		struct posix_acl **pacl, struct posix_acl **dpacl,
		unsigned int flags)
{
	struct posix_acl_state effective_acl_state, default_acl_state;
	struct nfs4_ace *ace;
	int ret;

	ret = init_state(&effective_acl_state, acl->naces);
	if (ret)
		return ret;
	ret = init_state(&default_acl_state, acl->naces);
	if (ret)
		goto out_estate;
	ret = -EINVAL;
	for (ace = acl->aces; ace < acl->aces + acl->naces; ace++) {
		if (ace->type != NFS4_ACE_ACCESS_ALLOWED_ACE_TYPE &&
		    ace->type != NFS4_ACE_ACCESS_DENIED_ACE_TYPE)
			goto out_dstate;
		if (ace->flag & ~NFS4_SUPPORTED_FLAGS)
			goto out_dstate;
		if ((ace->flag & NFS4_INHERITANCE_FLAGS) == 0) {
			process_one_v4_ace(&effective_acl_state, ace);
			continue;
		}
		if (!(flags & NFS4_ACL_DIR))
			goto out_dstate;
		/*
		 * Note that when only one of FILE_INHERIT or DIRECTORY_INHERIT
		 * is set, we're effectively turning on the other.  That's OK,
		 * according to rfc 3530.
		 */
		process_one_v4_ace(&default_acl_state, ace);

		if (!(ace->flag & NFS4_ACE_INHERIT_ONLY_ACE))
			process_one_v4_ace(&effective_acl_state, ace);
	}
	*pacl = posix_state_to_acl(&effective_acl_state, flags);
	if (IS_ERR(*pacl)) {
		ret = PTR_ERR(*pacl);
		*pacl = NULL;
		goto out_dstate;
	}
	*dpacl = posix_state_to_acl(&default_acl_state,
						flags | NFS4_ACL_TYPE_DEFAULT);
	if (IS_ERR(*dpacl)) {
		ret = PTR_ERR(*dpacl);
		*dpacl = NULL;
		posix_acl_release(*pacl);
		*pacl = NULL;
		goto out_dstate;
	}
	sort_pacl(*pacl);
	sort_pacl(*dpacl);
	ret = 0;
out_dstate:
	free_state(&default_acl_state);
out_estate:
	free_state(&effective_acl_state);
	return ret;
}
