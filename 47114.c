vhost_scsi_handle_vq(struct vhost_scsi *vs, struct vhost_virtqueue *vq)
{
	struct vhost_scsi_tpg **vs_tpg, *tpg;
	struct virtio_scsi_cmd_req v_req;
	struct virtio_scsi_cmd_req_pi v_req_pi;
	struct vhost_scsi_cmd *cmd;
	struct iov_iter out_iter, in_iter, prot_iter, data_iter;
	u64 tag;
	u32 exp_data_len, data_direction;
	unsigned out, in;
	int head, ret, prot_bytes;
	size_t req_size, rsp_size = sizeof(struct virtio_scsi_cmd_resp);
	size_t out_size, in_size;
	u16 lun;
	u8 *target, *lunp, task_attr;
	bool t10_pi = vhost_has_feature(vq, VIRTIO_SCSI_F_T10_PI);
	void *req, *cdb;

	mutex_lock(&vq->mutex);
	/*
	 * We can handle the vq only after the endpoint is setup by calling the
	 * VHOST_SCSI_SET_ENDPOINT ioctl.
	 */
	vs_tpg = vq->private_data;
	if (!vs_tpg)
		goto out;

	vhost_disable_notify(&vs->dev, vq);

	for (;;) {
		head = vhost_get_vq_desc(vq, vq->iov,
					 ARRAY_SIZE(vq->iov), &out, &in,
					 NULL, NULL);
		pr_debug("vhost_get_vq_desc: head: %d, out: %u in: %u\n",
			 head, out, in);
		/* On error, stop handling until the next kick. */
		if (unlikely(head < 0))
			break;
		/* Nothing new?  Wait for eventfd to tell us they refilled. */
		if (head == vq->num) {
			if (unlikely(vhost_enable_notify(&vs->dev, vq))) {
				vhost_disable_notify(&vs->dev, vq);
				continue;
			}
			break;
		}
		/*
		 * Check for a sane response buffer so we can report early
		 * errors back to the guest.
		 */
		if (unlikely(vq->iov[out].iov_len < rsp_size)) {
			vq_err(vq, "Expecting at least virtio_scsi_cmd_resp"
				" size, got %zu bytes\n", vq->iov[out].iov_len);
			break;
		}
		/*
		 * Setup pointers and values based upon different virtio-scsi
		 * request header if T10_PI is enabled in KVM guest.
		 */
		if (t10_pi) {
			req = &v_req_pi;
			req_size = sizeof(v_req_pi);
			lunp = &v_req_pi.lun[0];
			target = &v_req_pi.lun[1];
		} else {
			req = &v_req;
			req_size = sizeof(v_req);
			lunp = &v_req.lun[0];
			target = &v_req.lun[1];
		}
		/*
		 * FIXME: Not correct for BIDI operation
		 */
		out_size = iov_length(vq->iov, out);
		in_size = iov_length(&vq->iov[out], in);

		/*
		 * Copy over the virtio-scsi request header, which for a
		 * ANY_LAYOUT enabled guest may span multiple iovecs, or a
		 * single iovec may contain both the header + outgoing
		 * WRITE payloads.
		 *
		 * copy_from_iter() will advance out_iter, so that it will
		 * point at the start of the outgoing WRITE payload, if
		 * DMA_TO_DEVICE is set.
		 */
		iov_iter_init(&out_iter, WRITE, vq->iov, out, out_size);

		ret = copy_from_iter(req, req_size, &out_iter);
		if (unlikely(ret != req_size)) {
			vq_err(vq, "Faulted on copy_from_iter\n");
			vhost_scsi_send_bad_target(vs, vq, head, out);
			continue;
		}
		/* virtio-scsi spec requires byte 0 of the lun to be 1 */
		if (unlikely(*lunp != 1)) {
			vq_err(vq, "Illegal virtio-scsi lun: %u\n", *lunp);
			vhost_scsi_send_bad_target(vs, vq, head, out);
			continue;
		}

		tpg = ACCESS_ONCE(vs_tpg[*target]);
		if (unlikely(!tpg)) {
			/* Target does not exist, fail the request */
			vhost_scsi_send_bad_target(vs, vq, head, out);
			continue;
		}
		/*
		 * Determine data_direction by calculating the total outgoing
		 * iovec sizes + incoming iovec sizes vs. virtio-scsi request +
		 * response headers respectively.
		 *
		 * For DMA_TO_DEVICE this is out_iter, which is already pointing
		 * to the right place.
		 *
		 * For DMA_FROM_DEVICE, the iovec will be just past the end
		 * of the virtio-scsi response header in either the same
		 * or immediately following iovec.
		 *
		 * Any associated T10_PI bytes for the outgoing / incoming
		 * payloads are included in calculation of exp_data_len here.
		 */
		prot_bytes = 0;

		if (out_size > req_size) {
			data_direction = DMA_TO_DEVICE;
			exp_data_len = out_size - req_size;
			data_iter = out_iter;
		} else if (in_size > rsp_size) {
			data_direction = DMA_FROM_DEVICE;
			exp_data_len = in_size - rsp_size;

			iov_iter_init(&in_iter, READ, &vq->iov[out], in,
				      rsp_size + exp_data_len);
			iov_iter_advance(&in_iter, rsp_size);
			data_iter = in_iter;
		} else {
			data_direction = DMA_NONE;
			exp_data_len = 0;
		}
		/*
		 * If T10_PI header + payload is present, setup prot_iter values
		 * and recalculate data_iter for vhost_scsi_mapal() mapping to
		 * host scatterlists via get_user_pages_fast().
		 */
		if (t10_pi) {
			if (v_req_pi.pi_bytesout) {
				if (data_direction != DMA_TO_DEVICE) {
					vq_err(vq, "Received non zero pi_bytesout,"
						" but wrong data_direction\n");
					vhost_scsi_send_bad_target(vs, vq, head, out);
					continue;
				}
				prot_bytes = vhost32_to_cpu(vq, v_req_pi.pi_bytesout);
			} else if (v_req_pi.pi_bytesin) {
				if (data_direction != DMA_FROM_DEVICE) {
					vq_err(vq, "Received non zero pi_bytesin,"
						" but wrong data_direction\n");
					vhost_scsi_send_bad_target(vs, vq, head, out);
					continue;
				}
				prot_bytes = vhost32_to_cpu(vq, v_req_pi.pi_bytesin);
			}
			/*
			 * Set prot_iter to data_iter, and advance past any
			 * preceeding prot_bytes that may be present.
			 *
			 * Also fix up the exp_data_len to reflect only the
			 * actual data payload length.
			 */
			if (prot_bytes) {
				exp_data_len -= prot_bytes;
				prot_iter = data_iter;
				iov_iter_advance(&data_iter, prot_bytes);
			}
			tag = vhost64_to_cpu(vq, v_req_pi.tag);
			task_attr = v_req_pi.task_attr;
			cdb = &v_req_pi.cdb[0];
			lun = ((v_req_pi.lun[2] << 8) | v_req_pi.lun[3]) & 0x3FFF;
		} else {
			tag = vhost64_to_cpu(vq, v_req.tag);
			task_attr = v_req.task_attr;
			cdb = &v_req.cdb[0];
			lun = ((v_req.lun[2] << 8) | v_req.lun[3]) & 0x3FFF;
		}
		/*
		 * Check that the received CDB size does not exceeded our
		 * hardcoded max for vhost-scsi, then get a pre-allocated
		 * cmd descriptor for the new virtio-scsi tag.
		 *
		 * TODO what if cdb was too small for varlen cdb header?
		 */
		if (unlikely(scsi_command_size(cdb) > VHOST_SCSI_MAX_CDB_SIZE)) {
			vq_err(vq, "Received SCSI CDB with command_size: %d that"
				" exceeds SCSI_MAX_VARLEN_CDB_SIZE: %d\n",
				scsi_command_size(cdb), VHOST_SCSI_MAX_CDB_SIZE);
			vhost_scsi_send_bad_target(vs, vq, head, out);
			continue;
		}
		cmd = vhost_scsi_get_tag(vq, tpg, cdb, tag, lun, task_attr,
					 exp_data_len + prot_bytes,
					 data_direction);
		if (IS_ERR(cmd)) {
			vq_err(vq, "vhost_scsi_get_tag failed %ld\n",
			       PTR_ERR(cmd));
			vhost_scsi_send_bad_target(vs, vq, head, out);
			continue;
		}
		cmd->tvc_vhost = vs;
		cmd->tvc_vq = vq;
		cmd->tvc_resp_iov = &vq->iov[out];
		cmd->tvc_in_iovs = in;

		pr_debug("vhost_scsi got command opcode: %#02x, lun: %d\n",
			 cmd->tvc_cdb[0], cmd->tvc_lun);
		pr_debug("cmd: %p exp_data_len: %d, prot_bytes: %d data_direction:"
			 " %d\n", cmd, exp_data_len, prot_bytes, data_direction);

		if (data_direction != DMA_NONE) {
			ret = vhost_scsi_mapal(cmd,
					       prot_bytes, &prot_iter,
					       exp_data_len, &data_iter);
			if (unlikely(ret)) {
				vq_err(vq, "Failed to map iov to sgl\n");
				vhost_scsi_release_cmd(&cmd->tvc_se_cmd);
				vhost_scsi_send_bad_target(vs, vq, head, out);
				continue;
			}
		}
		/*
		 * Save the descriptor from vhost_get_vq_desc() to be used to
		 * complete the virtio-scsi request in TCM callback context via
		 * vhost_scsi_queue_data_in() and vhost_scsi_queue_status()
		 */
		cmd->tvc_vq_desc = head;
		/*
		 * Dispatch cmd descriptor for cmwq execution in process
		 * context provided by vhost_scsi_workqueue.  This also ensures
		 * cmd is executed on the same kworker CPU as this vhost
		 * thread to gain positive L2 cache locality effects.
		 */
		INIT_WORK(&cmd->work, vhost_scsi_submission_work);
		queue_work(vhost_scsi_workqueue, &cmd->work);
	}
out:
	mutex_unlock(&vq->mutex);
}
