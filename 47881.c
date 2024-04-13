static bool is_privileged_over(pid_t pid, uid_t uid, uid_t victim, bool req_ns_root)
{
	char fpath[PROCLEN];
	int ret;
	bool answer = false;
	uid_t nsuid;

	if (victim == -1 || uid == -1)
		return false;

	/*
	 * If the request is one not requiring root in the namespace,
	 * then having the same uid suffices.  (i.e. uid 1000 has write
	 * access to files owned by uid 1000
	 */
	if (!req_ns_root && uid == victim)
		return true;

	ret = snprintf(fpath, PROCLEN, "/proc/%d/uid_map", pid);
	if (ret < 0 || ret >= PROCLEN)
		return false;
	FILE *f = fopen(fpath, "r");
	if (!f)
		return false;

	/* if caller's not root in his namespace, reject */
	nsuid = convert_id_to_ns(f, uid);
	if (nsuid)
		goto out;

	/*
	 * If victim is not mapped into caller's ns, reject.
	 * XXX I'm not sure this check is needed given that fuse
	 * will be sending requests where the vfs has converted
	 */
	nsuid = convert_id_to_ns(f, victim);
	if (nsuid == -1)
		goto out;

	answer = true;

out:
	fclose(f);
	return answer;
}
