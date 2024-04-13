posix_state_to_acl(struct posix_acl_state *state, unsigned int flags)
{
	struct posix_acl_entry *pace;
	struct posix_acl *pacl;
	int nace;
	int i;

	/*
	 * ACLs with no ACEs are treated differently in the inheritable
	 * and effective cases: when there are no inheritable ACEs,
	 * calls ->set_acl with a NULL ACL structure.
	 */
	if (state->empty && (flags & NFS4_ACL_TYPE_DEFAULT))
		return NULL;

	/*
	 * When there are no effective ACEs, the following will end
	 * up setting a 3-element effective posix ACL with all
	 * permissions zero.
	 */
	if (!state->users->n && !state->groups->n)
		nace = 3;
	else /* Note we also include a MASK ACE in this case: */
		nace = 4 + state->users->n + state->groups->n;
	pacl = posix_acl_alloc(nace, GFP_KERNEL);
	if (!pacl)
		return ERR_PTR(-ENOMEM);

	pace = pacl->a_entries;
	pace->e_tag = ACL_USER_OBJ;
	low_mode_from_nfs4(state->owner.allow, &pace->e_perm, flags);

	for (i=0; i < state->users->n; i++) {
		pace++;
		pace->e_tag = ACL_USER;
		low_mode_from_nfs4(state->users->aces[i].perms.allow,
					&pace->e_perm, flags);
		pace->e_uid = state->users->aces[i].uid;
		add_to_mask(state, &state->users->aces[i].perms);
	}

	pace++;
	pace->e_tag = ACL_GROUP_OBJ;
	low_mode_from_nfs4(state->group.allow, &pace->e_perm, flags);
	add_to_mask(state, &state->group);

	for (i=0; i < state->groups->n; i++) {
		pace++;
		pace->e_tag = ACL_GROUP;
		low_mode_from_nfs4(state->groups->aces[i].perms.allow,
					&pace->e_perm, flags);
		pace->e_gid = state->groups->aces[i].gid;
		add_to_mask(state, &state->groups->aces[i].perms);
	}

	if (state->users->n || state->groups->n) {
		pace++;
		pace->e_tag = ACL_MASK;
		low_mode_from_nfs4(state->mask.allow, &pace->e_perm, flags);
	}

	pace++;
	pace->e_tag = ACL_OTHER;
	low_mode_from_nfs4(state->other.allow, &pace->e_perm, flags);

	return pacl;
}
