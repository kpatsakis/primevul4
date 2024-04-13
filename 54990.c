static void __bnep_unlink_session(struct bnep_session *s)
{
	list_del(&s->list);
}
