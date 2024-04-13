static int msgctl_nolock(struct ipc_namespace *ns, int msqid,
			 int cmd, int version, void __user *buf)
{
	int err;
	struct msg_queue *msq;

	switch (cmd) {
	case IPC_INFO:
	case MSG_INFO:
	{
		struct msginfo msginfo;
		int max_id;

		if (!buf)
			return -EFAULT;

		/*
		 * We must not return kernel stack data.
		 * due to padding, it's not enough
		 * to set all member fields.
		 */
		err = security_msg_queue_msgctl(NULL, cmd);
		if (err)
			return err;

		memset(&msginfo, 0, sizeof(msginfo));
		msginfo.msgmni = ns->msg_ctlmni;
		msginfo.msgmax = ns->msg_ctlmax;
		msginfo.msgmnb = ns->msg_ctlmnb;
		msginfo.msgssz = MSGSSZ;
		msginfo.msgseg = MSGSEG;
		down_read(&msg_ids(ns).rwsem);
		if (cmd == MSG_INFO) {
			msginfo.msgpool = msg_ids(ns).in_use;
			msginfo.msgmap = atomic_read(&ns->msg_hdrs);
			msginfo.msgtql = atomic_read(&ns->msg_bytes);
		} else {
			msginfo.msgmap = MSGMAP;
			msginfo.msgpool = MSGPOOL;
			msginfo.msgtql = MSGTQL;
		}
		max_id = ipc_get_maxid(&msg_ids(ns));
		up_read(&msg_ids(ns).rwsem);
		if (copy_to_user(buf, &msginfo, sizeof(struct msginfo)))
			return -EFAULT;
		return (max_id < 0) ? 0 : max_id;
	}

	case MSG_STAT:
	case IPC_STAT:
	{
		struct msqid64_ds tbuf;
		int success_return;

		if (!buf)
			return -EFAULT;

		memset(&tbuf, 0, sizeof(tbuf));

		rcu_read_lock();
		if (cmd == MSG_STAT) {
			msq = msq_obtain_object(ns, msqid);
			if (IS_ERR(msq)) {
				err = PTR_ERR(msq);
				goto out_unlock;
			}
			success_return = msq->q_perm.id;
		} else {
			msq = msq_obtain_object_check(ns, msqid);
			if (IS_ERR(msq)) {
				err = PTR_ERR(msq);
				goto out_unlock;
			}
			success_return = 0;
		}

		err = -EACCES;
		if (ipcperms(ns, &msq->q_perm, S_IRUGO))
			goto out_unlock;

		err = security_msg_queue_msgctl(msq, cmd);
		if (err)
			goto out_unlock;

		kernel_to_ipc64_perm(&msq->q_perm, &tbuf.msg_perm);
		tbuf.msg_stime  = msq->q_stime;
		tbuf.msg_rtime  = msq->q_rtime;
		tbuf.msg_ctime  = msq->q_ctime;
		tbuf.msg_cbytes = msq->q_cbytes;
		tbuf.msg_qnum   = msq->q_qnum;
		tbuf.msg_qbytes = msq->q_qbytes;
		tbuf.msg_lspid  = msq->q_lspid;
		tbuf.msg_lrpid  = msq->q_lrpid;
		rcu_read_unlock();

		if (copy_msqid_to_user(buf, &tbuf, version))
			return -EFAULT;
		return success_return;
	}

	default:
		return -EINVAL;
	}

	return err;
out_unlock:
	rcu_read_unlock();
	return err;
}
