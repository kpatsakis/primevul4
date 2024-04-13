int security_fs_use(struct super_block *sb)
{
	int rc = 0;
	struct ocontext *c;
	struct superblock_security_struct *sbsec = sb->s_security;
	const char *fstype = sb->s_type->name;

	read_lock(&policy_rwlock);

	c = policydb.ocontexts[OCON_FSUSE];
	while (c) {
		if (strcmp(fstype, c->u.name) == 0)
			break;
		c = c->next;
	}

	if (c) {
		sbsec->behavior = c->v.behavior;
		if (!c->sid[0]) {
			rc = sidtab_context_to_sid(&sidtab, &c->context[0],
						   &c->sid[0]);
			if (rc)
				goto out;
		}
		sbsec->sid = c->sid[0];
	} else {
		rc = security_genfs_sid(fstype, "/", SECCLASS_DIR, &sbsec->sid);
		if (rc) {
			sbsec->behavior = SECURITY_FS_USE_NONE;
			rc = 0;
		} else {
			sbsec->behavior = SECURITY_FS_USE_GENFS;
		}
	}

out:
	read_unlock(&policy_rwlock);
	return rc;
}
