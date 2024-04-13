vhost_scsi_set_endpoint(struct vhost_scsi *vs,
			struct vhost_scsi_target *t)
{
	struct se_portal_group *se_tpg;
	struct vhost_scsi_tport *tv_tport;
	struct vhost_scsi_tpg *tpg;
	struct vhost_scsi_tpg **vs_tpg;
	struct vhost_virtqueue *vq;
	int index, ret, i, len;
	bool match = false;

	mutex_lock(&vhost_scsi_mutex);
	mutex_lock(&vs->dev.mutex);

	/* Verify that ring has been setup correctly. */
	for (index = 0; index < vs->dev.nvqs; ++index) {
		/* Verify that ring has been setup correctly. */
		if (!vhost_vq_access_ok(&vs->vqs[index].vq)) {
			ret = -EFAULT;
			goto out;
		}
	}

	len = sizeof(vs_tpg[0]) * VHOST_SCSI_MAX_TARGET;
	vs_tpg = kzalloc(len, GFP_KERNEL);
	if (!vs_tpg) {
		ret = -ENOMEM;
		goto out;
	}
	if (vs->vs_tpg)
		memcpy(vs_tpg, vs->vs_tpg, len);

	list_for_each_entry(tpg, &vhost_scsi_list, tv_tpg_list) {
		mutex_lock(&tpg->tv_tpg_mutex);
		if (!tpg->tpg_nexus) {
			mutex_unlock(&tpg->tv_tpg_mutex);
			continue;
		}
		if (tpg->tv_tpg_vhost_count != 0) {
			mutex_unlock(&tpg->tv_tpg_mutex);
			continue;
		}
		tv_tport = tpg->tport;

		if (!strcmp(tv_tport->tport_name, t->vhost_wwpn)) {
			if (vs->vs_tpg && vs->vs_tpg[tpg->tport_tpgt]) {
				kfree(vs_tpg);
				mutex_unlock(&tpg->tv_tpg_mutex);
				ret = -EEXIST;
				goto out;
			}
			/*
			 * In order to ensure individual vhost-scsi configfs
			 * groups cannot be removed while in use by vhost ioctl,
			 * go ahead and take an explicit se_tpg->tpg_group.cg_item
			 * dependency now.
			 */
			se_tpg = &tpg->se_tpg;
			ret = configfs_depend_item(se_tpg->se_tpg_tfo->tf_subsys,
						   &se_tpg->tpg_group.cg_item);
			if (ret) {
				pr_warn("configfs_depend_item() failed: %d\n", ret);
				kfree(vs_tpg);
				mutex_unlock(&tpg->tv_tpg_mutex);
				goto out;
			}
			tpg->tv_tpg_vhost_count++;
			tpg->vhost_scsi = vs;
			vs_tpg[tpg->tport_tpgt] = tpg;
			smp_mb__after_atomic();
			match = true;
		}
		mutex_unlock(&tpg->tv_tpg_mutex);
	}

	if (match) {
		memcpy(vs->vs_vhost_wwpn, t->vhost_wwpn,
		       sizeof(vs->vs_vhost_wwpn));
		for (i = 0; i < VHOST_SCSI_MAX_VQ; i++) {
			vq = &vs->vqs[i].vq;
			mutex_lock(&vq->mutex);
			vq->private_data = vs_tpg;
			vhost_init_used(vq);
			mutex_unlock(&vq->mutex);
		}
		ret = 0;
	} else {
		ret = -EEXIST;
	}

	/*
	 * Act as synchronize_rcu to make sure access to
	 * old vs->vs_tpg is finished.
	 */
	vhost_scsi_flush(vs);
	kfree(vs->vs_tpg);
	vs->vs_tpg = vs_tpg;

out:
	mutex_unlock(&vs->dev.mutex);
	mutex_unlock(&vhost_scsi_mutex);
	return ret;
}
