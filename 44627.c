int security_load_policy(void *data, size_t len)
{
	struct policydb *oldpolicydb, *newpolicydb;
	struct sidtab oldsidtab, newsidtab;
	struct selinux_mapping *oldmap, *map = NULL;
	struct convert_context_args args;
	u32 seqno;
	u16 map_size;
	int rc = 0;
	struct policy_file file = { data, len }, *fp = &file;

	oldpolicydb = kzalloc(2 * sizeof(*oldpolicydb), GFP_KERNEL);
	if (!oldpolicydb) {
		rc = -ENOMEM;
		goto out;
	}
	newpolicydb = oldpolicydb + 1;

	if (!ss_initialized) {
		avtab_cache_init();
		rc = policydb_read(&policydb, fp);
		if (rc) {
			avtab_cache_destroy();
			goto out;
		}

		policydb.len = len;
		rc = selinux_set_mapping(&policydb, secclass_map,
					 &current_mapping,
					 &current_mapping_size);
		if (rc) {
			policydb_destroy(&policydb);
			avtab_cache_destroy();
			goto out;
		}

		rc = policydb_load_isids(&policydb, &sidtab);
		if (rc) {
			policydb_destroy(&policydb);
			avtab_cache_destroy();
			goto out;
		}

		security_load_policycaps();
		ss_initialized = 1;
		seqno = ++latest_granting;
		selinux_complete_init();
		avc_ss_reset(seqno);
		selnl_notify_policyload(seqno);
		selinux_status_update_policyload(seqno);
		selinux_netlbl_cache_invalidate();
		selinux_xfrm_notify_policyload();
		goto out;
	}

#if 0
	sidtab_hash_eval(&sidtab, "sids");
#endif

	rc = policydb_read(newpolicydb, fp);
	if (rc)
		goto out;

	newpolicydb->len = len;
	/* If switching between different policy types, log MLS status */
	if (policydb.mls_enabled && !newpolicydb->mls_enabled)
		printk(KERN_INFO "SELinux: Disabling MLS support...\n");
	else if (!policydb.mls_enabled && newpolicydb->mls_enabled)
		printk(KERN_INFO "SELinux: Enabling MLS support...\n");

	rc = policydb_load_isids(newpolicydb, &newsidtab);
	if (rc) {
		printk(KERN_ERR "SELinux:  unable to load the initial SIDs\n");
		policydb_destroy(newpolicydb);
		goto out;
	}

	rc = selinux_set_mapping(newpolicydb, secclass_map, &map, &map_size);
	if (rc)
		goto err;

	rc = security_preserve_bools(newpolicydb);
	if (rc) {
		printk(KERN_ERR "SELinux:  unable to preserve booleans\n");
		goto err;
	}

	/* Clone the SID table. */
	sidtab_shutdown(&sidtab);

	rc = sidtab_map(&sidtab, clone_sid, &newsidtab);
	if (rc)
		goto err;

	/*
	 * Convert the internal representations of contexts
	 * in the new SID table.
	 */
	args.oldp = &policydb;
	args.newp = newpolicydb;
	rc = sidtab_map(&newsidtab, convert_context, &args);
	if (rc) {
		printk(KERN_ERR "SELinux:  unable to convert the internal"
			" representation of contexts in the new SID"
			" table\n");
		goto err;
	}

	/* Save the old policydb and SID table to free later. */
	memcpy(oldpolicydb, &policydb, sizeof(policydb));
	sidtab_set(&oldsidtab, &sidtab);

	/* Install the new policydb and SID table. */
	write_lock_irq(&policy_rwlock);
	memcpy(&policydb, newpolicydb, sizeof(policydb));
	sidtab_set(&sidtab, &newsidtab);
	security_load_policycaps();
	oldmap = current_mapping;
	current_mapping = map;
	current_mapping_size = map_size;
	seqno = ++latest_granting;
	write_unlock_irq(&policy_rwlock);

	/* Free the old policydb and SID table. */
	policydb_destroy(oldpolicydb);
	sidtab_destroy(&oldsidtab);
	kfree(oldmap);

	avc_ss_reset(seqno);
	selnl_notify_policyload(seqno);
	selinux_status_update_policyload(seqno);
	selinux_netlbl_cache_invalidate();
	selinux_xfrm_notify_policyload();

	rc = 0;
	goto out;

err:
	kfree(map);
	sidtab_destroy(&newsidtab);
	policydb_destroy(newpolicydb);

out:
	kfree(oldpolicydb);
	return rc;
}
