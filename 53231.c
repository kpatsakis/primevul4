static int find_gid(struct posix_acl_state *state, kgid_t gid)
{
	struct posix_ace_state_array *a = state->groups;
	int i;

	for (i = 0; i < a->n; i++)
		if (gid_eq(a->aces[i].gid, gid))
			return i;
	/* Not found: */
	a->n++;
	a->aces[i].gid = gid;
	a->aces[i].perms.allow = state->everyone.allow;
	a->aces[i].perms.deny  = state->everyone.deny;

	return i;
}
