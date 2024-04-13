static int clone_sid(u32 sid,
		     struct context *context,
		     void *arg)
{
	struct sidtab *s = arg;

	if (sid > SECINITSID_NUM)
		return sidtab_insert(s, sid, context);
	else
		return 0;
}
