void security_compute_av(u32 ssid,
			 u32 tsid,
			 u16 orig_tclass,
			 struct av_decision *avd)
{
	u16 tclass;
	struct context *scontext = NULL, *tcontext = NULL;

	read_lock(&policy_rwlock);
	avd_init(avd);
	if (!ss_initialized)
		goto allow;

	scontext = sidtab_search(&sidtab, ssid);
	if (!scontext) {
		printk(KERN_ERR "SELinux: %s:  unrecognized SID %d\n",
		       __func__, ssid);
		goto out;
	}

	/* permissive domain? */
	if (ebitmap_get_bit(&policydb.permissive_map, scontext->type))
		avd->flags |= AVD_FLAGS_PERMISSIVE;

	tcontext = sidtab_search(&sidtab, tsid);
	if (!tcontext) {
		printk(KERN_ERR "SELinux: %s:  unrecognized SID %d\n",
		       __func__, tsid);
		goto out;
	}

	tclass = unmap_class(orig_tclass);
	if (unlikely(orig_tclass && !tclass)) {
		if (policydb.allow_unknown)
			goto allow;
		goto out;
	}
	context_struct_compute_av(scontext, tcontext, tclass, avd);
	map_decision(orig_tclass, avd, policydb.allow_unknown);
out:
	read_unlock(&policy_rwlock);
	return;
allow:
	avd->allowed = 0xffffffff;
	goto out;
}
