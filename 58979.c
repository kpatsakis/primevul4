static struct ucounts *find_ucounts(struct user_namespace *ns, kuid_t uid, struct hlist_head *hashent)
{
	struct ucounts *ucounts;

	hlist_for_each_entry(ucounts, hashent, node) {
		if (uid_eq(ucounts->uid, uid) && (ucounts->ns == ns))
			return ucounts;
	}
	return NULL;
}
