static bool caller_is_in_ancestor(pid_t pid, const char *contrl, const char *cg, char **nextcg)
{
	bool answer = false;
	char *c2 = get_pid_cgroup(pid, contrl);
	char *linecmp;

	if (!c2)
		return false;
	prune_init_slice(c2);

	/*
	 * callers pass in '/' for root cgroup, otherwise they pass
	 * in a cgroup without leading '/'
	 */
	linecmp = *cg == '/' ? c2 : c2+1;
	if (strncmp(linecmp, cg, strlen(linecmp)) != 0) {
		if (nextcg) {
			*nextcg = get_next_cgroup_dir(linecmp, cg);
		}
		goto out;
	}
	answer = true;

out:
	free(c2);
	return answer;
}
