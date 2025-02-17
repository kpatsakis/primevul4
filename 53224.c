_posix_to_nfsv4_one(struct posix_acl *pacl, struct nfs4_acl *acl,
						unsigned int flags)
{
	struct posix_acl_entry *pa, *group_owner_entry;
	struct nfs4_ace *ace;
	struct posix_acl_summary pas;
	unsigned short deny;
	int eflag = ((flags & NFS4_ACL_TYPE_DEFAULT) ?
		NFS4_INHERITANCE_FLAGS | NFS4_ACE_INHERIT_ONLY_ACE : 0);

	BUG_ON(pacl->a_count < 3);
	summarize_posix_acl(pacl, &pas);

	pa = pacl->a_entries;
	ace = acl->aces + acl->naces;

	/* We could deny everything not granted by the owner: */
	deny = ~pas.owner;
	/*
	 * but it is equivalent (and simpler) to deny only what is not
	 * granted by later entries:
	 */
	deny &= pas.users | pas.group | pas.groups | pas.other;
	if (deny) {
		ace->type = NFS4_ACE_ACCESS_DENIED_ACE_TYPE;
		ace->flag = eflag;
		ace->access_mask = deny_mask_from_posix(deny, flags);
		ace->whotype = NFS4_ACL_WHO_OWNER;
		ace++;
		acl->naces++;
	}

	ace->type = NFS4_ACE_ACCESS_ALLOWED_ACE_TYPE;
	ace->flag = eflag;
	ace->access_mask = mask_from_posix(pa->e_perm, flags | NFS4_ACL_OWNER);
	ace->whotype = NFS4_ACL_WHO_OWNER;
	ace++;
	acl->naces++;
	pa++;

	while (pa->e_tag == ACL_USER) {
		deny = ~(pa->e_perm & pas.mask);
		deny &= pas.groups | pas.group | pas.other;
		if (deny) {
			ace->type = NFS4_ACE_ACCESS_DENIED_ACE_TYPE;
			ace->flag = eflag;
			ace->access_mask = deny_mask_from_posix(deny, flags);
			ace->whotype = NFS4_ACL_WHO_NAMED;
			ace->who_uid = pa->e_uid;
			ace++;
			acl->naces++;
		}
		ace->type = NFS4_ACE_ACCESS_ALLOWED_ACE_TYPE;
		ace->flag = eflag;
		ace->access_mask = mask_from_posix(pa->e_perm & pas.mask,
						   flags);
		ace->whotype = NFS4_ACL_WHO_NAMED;
		ace->who_uid = pa->e_uid;
		ace++;
		acl->naces++;
		pa++;
	}

	/* In the case of groups, we apply allow ACEs first, then deny ACEs,
	 * since a user can be in more than one group.  */

	/* allow ACEs */

	group_owner_entry = pa;

	ace->type = NFS4_ACE_ACCESS_ALLOWED_ACE_TYPE;
	ace->flag = eflag;
	ace->access_mask = mask_from_posix(pas.group, flags);
	ace->whotype = NFS4_ACL_WHO_GROUP;
	ace++;
	acl->naces++;
	pa++;

	while (pa->e_tag == ACL_GROUP) {
		ace->type = NFS4_ACE_ACCESS_ALLOWED_ACE_TYPE;
		ace->flag = eflag | NFS4_ACE_IDENTIFIER_GROUP;
		ace->access_mask = mask_from_posix(pa->e_perm & pas.mask,
						   flags);
		ace->whotype = NFS4_ACL_WHO_NAMED;
		ace->who_gid = pa->e_gid;
		ace++;
		acl->naces++;
		pa++;
	}

	/* deny ACEs */

	pa = group_owner_entry;

	deny = ~pas.group & pas.other;
	if (deny) {
		ace->type = NFS4_ACE_ACCESS_DENIED_ACE_TYPE;
		ace->flag = eflag;
		ace->access_mask = deny_mask_from_posix(deny, flags);
		ace->whotype = NFS4_ACL_WHO_GROUP;
		ace++;
		acl->naces++;
	}
	pa++;

	while (pa->e_tag == ACL_GROUP) {
		deny = ~(pa->e_perm & pas.mask);
		deny &= pas.other;
		if (deny) {
			ace->type = NFS4_ACE_ACCESS_DENIED_ACE_TYPE;
			ace->flag = eflag | NFS4_ACE_IDENTIFIER_GROUP;
			ace->access_mask = deny_mask_from_posix(deny, flags);
			ace->whotype = NFS4_ACL_WHO_NAMED;
			ace->who_gid = pa->e_gid;
			ace++;
			acl->naces++;
		}
		pa++;
	}

	if (pa->e_tag == ACL_MASK)
		pa++;
	ace->type = NFS4_ACE_ACCESS_ALLOWED_ACE_TYPE;
	ace->flag = eflag;
	ace->access_mask = mask_from_posix(pa->e_perm, flags);
	ace->whotype = NFS4_ACL_WHO_EVERYONE;
	acl->naces++;
}
