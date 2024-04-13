static void mcryptd_opportunistic_flush(void)
{
	struct mcryptd_flush_list *flist;
	struct mcryptd_alg_cstate *cstate;

	flist = per_cpu_ptr(mcryptd_flist, smp_processor_id());
	while (single_task_running()) {
		mutex_lock(&flist->lock);
		if (list_empty(&flist->list)) {
			mutex_unlock(&flist->lock);
			return;
		}
		cstate = list_entry(flist->list.next,
				struct mcryptd_alg_cstate, flush_list);
		if (!cstate->flusher_engaged) {
			mutex_unlock(&flist->lock);
			return;
		}
		list_del(&cstate->flush_list);
		cstate->flusher_engaged = false;
		mutex_unlock(&flist->lock);
		cstate->alg_state->flusher(cstate);
	}
}
