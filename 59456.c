vc4_save_hang_state(struct drm_device *dev)
{
	struct vc4_dev *vc4 = to_vc4_dev(dev);
	struct drm_vc4_get_hang_state *state;
	struct vc4_hang_state *kernel_state;
	struct vc4_exec_info *exec[2];
	struct vc4_bo *bo;
	unsigned long irqflags;
	unsigned int i, j, unref_list_count, prev_idx;

	kernel_state = kcalloc(1, sizeof(*kernel_state), GFP_KERNEL);
	if (!kernel_state)
		return;

	state = &kernel_state->user_state;

	spin_lock_irqsave(&vc4->job_lock, irqflags);
	exec[0] = vc4_first_bin_job(vc4);
	exec[1] = vc4_first_render_job(vc4);
	if (!exec[0] && !exec[1]) {
		spin_unlock_irqrestore(&vc4->job_lock, irqflags);
		return;
	}

	/* Get the bos from both binner and renderer into hang state. */
	state->bo_count = 0;
	for (i = 0; i < 2; i++) {
		if (!exec[i])
			continue;

		unref_list_count = 0;
		list_for_each_entry(bo, &exec[i]->unref_list, unref_head)
			unref_list_count++;
		state->bo_count += exec[i]->bo_count + unref_list_count;
	}

	kernel_state->bo = kcalloc(state->bo_count,
				   sizeof(*kernel_state->bo), GFP_ATOMIC);

	if (!kernel_state->bo) {
		spin_unlock_irqrestore(&vc4->job_lock, irqflags);
		return;
	}

	prev_idx = 0;
	for (i = 0; i < 2; i++) {
		if (!exec[i])
			continue;

		for (j = 0; j < exec[i]->bo_count; j++) {
			drm_gem_object_reference(&exec[i]->bo[j]->base);
			kernel_state->bo[j + prev_idx] = &exec[i]->bo[j]->base;
		}

		list_for_each_entry(bo, &exec[i]->unref_list, unref_head) {
			drm_gem_object_reference(&bo->base.base);
			kernel_state->bo[j + prev_idx] = &bo->base.base;
			j++;
		}
		prev_idx = j + 1;
	}

	if (exec[0])
		state->start_bin = exec[0]->ct0ca;
	if (exec[1])
		state->start_render = exec[1]->ct1ca;

	spin_unlock_irqrestore(&vc4->job_lock, irqflags);

	state->ct0ca = V3D_READ(V3D_CTNCA(0));
	state->ct0ea = V3D_READ(V3D_CTNEA(0));

	state->ct1ca = V3D_READ(V3D_CTNCA(1));
	state->ct1ea = V3D_READ(V3D_CTNEA(1));

	state->ct0cs = V3D_READ(V3D_CTNCS(0));
	state->ct1cs = V3D_READ(V3D_CTNCS(1));

	state->ct0ra0 = V3D_READ(V3D_CT00RA0);
	state->ct1ra0 = V3D_READ(V3D_CT01RA0);

	state->bpca = V3D_READ(V3D_BPCA);
	state->bpcs = V3D_READ(V3D_BPCS);
	state->bpoa = V3D_READ(V3D_BPOA);
	state->bpos = V3D_READ(V3D_BPOS);

	state->vpmbase = V3D_READ(V3D_VPMBASE);

	state->dbge = V3D_READ(V3D_DBGE);
	state->fdbgo = V3D_READ(V3D_FDBGO);
	state->fdbgb = V3D_READ(V3D_FDBGB);
	state->fdbgr = V3D_READ(V3D_FDBGR);
	state->fdbgs = V3D_READ(V3D_FDBGS);
	state->errstat = V3D_READ(V3D_ERRSTAT);

	spin_lock_irqsave(&vc4->job_lock, irqflags);
	if (vc4->hang_state) {
		spin_unlock_irqrestore(&vc4->job_lock, irqflags);
		vc4_free_hang_state(dev, kernel_state);
	} else {
		vc4->hang_state = kernel_state;
		spin_unlock_irqrestore(&vc4->job_lock, irqflags);
	}
}
