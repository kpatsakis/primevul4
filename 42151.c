static void __hidp_link_session(struct hidp_session *session)
{
	list_add(&session->list, &hidp_session_list);
}
