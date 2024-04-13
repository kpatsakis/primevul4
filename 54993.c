int bnep_del_connection(struct bnep_conndel_req *req)
{
	struct bnep_session *s;
	int  err = 0;

	BT_DBG("");

	down_read(&bnep_session_sem);

	s = __bnep_get_session(req->dst);
	if (s) {
		atomic_inc(&s->terminate);
		wake_up_process(s->task);
	} else
		err = -ENOENT;

	up_read(&bnep_session_sem);
	return err;
}
