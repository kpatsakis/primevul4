int bnep_get_connlist(struct bnep_connlist_req *req)
{
	struct bnep_session *s;
	int err = 0, n = 0;

	down_read(&bnep_session_sem);

	list_for_each_entry(s, &bnep_session_list, list) {
		struct bnep_conninfo ci;

		__bnep_copy_ci(&ci, s);

		if (copy_to_user(req->ci, &ci, sizeof(ci))) {
			err = -EFAULT;
			break;
		}

		if (++n >= req->cnum)
			break;

		req->ci++;
	}
	req->cnum = n;

	up_read(&bnep_session_sem);
	return err;
}
