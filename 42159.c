int hidp_get_connlist(struct hidp_connlist_req *req)
{
	struct hidp_session *session;
	int err = 0, n = 0;

	BT_DBG("");

	down_read(&hidp_session_sem);

	list_for_each_entry(session, &hidp_session_list, list) {
		struct hidp_conninfo ci;

		__hidp_copy_session(session, &ci);

		if (copy_to_user(req->ci, &ci, sizeof(ci))) {
			err = -EFAULT;
			break;
		}

		if (++n >= req->cnum)
			break;

		req->ci++;
	}
	req->cnum = n;

	up_read(&hidp_session_sem);
	return err;
}
