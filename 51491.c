static int apparmor_capget(struct task_struct *target, kernel_cap_t *effective,
			   kernel_cap_t *inheritable, kernel_cap_t *permitted)
{
	struct aa_profile *profile;
	const struct cred *cred;

	rcu_read_lock();
	cred = __task_cred(target);
	profile = aa_cred_profile(cred);

	/*
	 * cap_capget is stacked ahead of this and will
	 * initialize effective and permitted.
	 */
	if (!unconfined(profile) && !COMPLAIN_MODE(profile)) {
		*effective = cap_intersect(*effective, profile->caps.allow);
		*permitted = cap_intersect(*permitted, profile->caps.allow);
	}
	rcu_read_unlock();

	return 0;
}
