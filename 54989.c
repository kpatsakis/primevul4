static void __bnep_link_session(struct bnep_session *s)
{
	list_add(&s->list, &bnep_session_list);
}
