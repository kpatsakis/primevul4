sg_get_rq_mark(Sg_fd * sfp, int pack_id)
{
	Sg_request *resp;
	unsigned long iflags;

	write_lock_irqsave(&sfp->rq_list_lock, iflags);
	list_for_each_entry(resp, &sfp->rq_list, entry) {
		/* look for requests that are ready + not SG_IO owned */
		if ((1 == resp->done) && (!resp->sg_io_owned) &&
		    ((-1 == pack_id) || (resp->header.pack_id == pack_id))) {
			resp->done = 2;	/* guard against other readers */
			write_unlock_irqrestore(&sfp->rq_list_lock, iflags);
			return resp;
		}
	}
	write_unlock_irqrestore(&sfp->rq_list_lock, iflags);
	return NULL;
}
