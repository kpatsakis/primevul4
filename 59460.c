vc4_update_bo_seqnos(struct vc4_exec_info *exec, uint64_t seqno)
{
	struct vc4_bo *bo;
	unsigned i;

	for (i = 0; i < exec->bo_count; i++) {
		bo = to_vc4_bo(&exec->bo[i]->base);
		bo->seqno = seqno;
	}

	list_for_each_entry(bo, &exec->unref_list, unref_head) {
		bo->seqno = seqno;
	}

	for (i = 0; i < exec->rcl_write_bo_count; i++) {
		bo = to_vc4_bo(&exec->rcl_write_bo[i]->base);
		bo->write_seqno = seqno;
	}
}
