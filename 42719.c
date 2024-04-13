cifs_demultiplex_thread(struct TCP_Server_Info *server)
{
	int length;
	unsigned int pdu_length, total_read;
	struct smb_hdr *smb_buffer = NULL;
	struct smb_hdr *bigbuf = NULL;
	struct smb_hdr *smallbuf = NULL;
	struct msghdr smb_msg;
	struct kvec iov;
	struct socket *csocket = server->ssocket;
	struct list_head *tmp;
	struct cifsSesInfo *ses;
	struct task_struct *task_to_wake = NULL;
	struct mid_q_entry *mid_entry;
	char temp;
	bool isLargeBuf = false;
	bool isMultiRsp;
	int reconnect;

	current->flags |= PF_MEMALLOC;
	cFYI(1, "Demultiplex PID: %d", task_pid_nr(current));

	length = atomic_inc_return(&tcpSesAllocCount);
	if (length > 1)
		mempool_resize(cifs_req_poolp, length + cifs_min_rcv,
				GFP_KERNEL);

	set_freezable();
	while (server->tcpStatus != CifsExiting) {
		if (try_to_freeze())
			continue;
		if (bigbuf == NULL) {
			bigbuf = cifs_buf_get();
			if (!bigbuf) {
				cERROR(1, "No memory for large SMB response");
				msleep(3000);
				/* retry will check if exiting */
				continue;
			}
		} else if (isLargeBuf) {
			/* we are reusing a dirty large buf, clear its start */
			memset(bigbuf, 0, sizeof(struct smb_hdr));
		}

		if (smallbuf == NULL) {
			smallbuf = cifs_small_buf_get();
			if (!smallbuf) {
				cERROR(1, "No memory for SMB response");
				msleep(1000);
				/* retry will check if exiting */
				continue;
			}
			/* beginning of smb buffer is cleared in our buf_get */
		} else /* if existing small buf clear beginning */
			memset(smallbuf, 0, sizeof(struct smb_hdr));

		isLargeBuf = false;
		isMultiRsp = false;
		smb_buffer = smallbuf;
		iov.iov_base = smb_buffer;
		iov.iov_len = 4;
		smb_msg.msg_control = NULL;
		smb_msg.msg_controllen = 0;
		pdu_length = 4; /* enough to get RFC1001 header */
incomplete_rcv:
		length =
		    kernel_recvmsg(csocket, &smb_msg,
				&iov, 1, pdu_length, 0 /* BB other flags? */);

		if (server->tcpStatus == CifsExiting) {
			break;
		} else if (server->tcpStatus == CifsNeedReconnect) {
			cFYI(1, "Reconnect after server stopped responding");
			cifs_reconnect(server);
			cFYI(1, "call to reconnect done");
			csocket = server->ssocket;
			continue;
		} else if ((length == -ERESTARTSYS) || (length == -EAGAIN)) {
			msleep(1); /* minimum sleep to prevent looping
				allowing socket to clear and app threads to set
				tcpStatus CifsNeedReconnect if server hung */
			if (pdu_length < 4) {
				iov.iov_base = (4 - pdu_length) +
							(char *)smb_buffer;
				iov.iov_len = pdu_length;
				smb_msg.msg_control = NULL;
				smb_msg.msg_controllen = 0;
				goto incomplete_rcv;
			} else
				continue;
		} else if (length <= 0) {
			if (server->tcpStatus == CifsNew) {
				cFYI(1, "tcp session abend after SMBnegprot");
				/* some servers kill the TCP session rather than
				   returning an SMB negprot error, in which
				   case reconnecting here is not going to help,
				   and so simply return error to mount */
				break;
			}
			if (!try_to_freeze() && (length == -EINTR)) {
				cFYI(1, "cifsd thread killed");
				break;
			}
			cFYI(1, "Reconnect after unexpected peek error %d",
				length);
			cifs_reconnect(server);
			csocket = server->ssocket;
			wake_up(&server->response_q);
			continue;
		} else if (length < pdu_length) {
			cFYI(1, "requested %d bytes but only got %d bytes",
				  pdu_length, length);
			pdu_length -= length;
			msleep(1);
			goto incomplete_rcv;
		}

		/* The right amount was read from socket - 4 bytes */
		/* so we can now interpret the length field */

		/* the first byte big endian of the length field,
		is actually not part of the length but the type
		with the most common, zero, as regular data */
		temp = *((char *) smb_buffer);

		/* Note that FC 1001 length is big endian on the wire,
		but we convert it here so it is always manipulated
		as host byte order */
		pdu_length = be32_to_cpu((__force __be32)smb_buffer->smb_buf_length);
		smb_buffer->smb_buf_length = pdu_length;

		cFYI(1, "rfc1002 length 0x%x", pdu_length+4);

		if (temp == (char) RFC1002_SESSION_KEEP_ALIVE) {
			continue;
		} else if (temp == (char)RFC1002_POSITIVE_SESSION_RESPONSE) {
			cFYI(1, "Good RFC 1002 session rsp");
			continue;
		} else if (temp == (char)RFC1002_NEGATIVE_SESSION_RESPONSE) {
			/* we get this from Windows 98 instead of
			   an error on SMB negprot response */
			cFYI(1, "Negative RFC1002 Session Response Error 0x%x)",
				pdu_length);
			if (server->tcpStatus == CifsNew) {
				/* if nack on negprot (rather than
				ret of smb negprot error) reconnecting
				not going to help, ret error to mount */
				break;
			} else {
				/* give server a second to
				clean up before reconnect attempt */
				msleep(1000);
				/* always try 445 first on reconnect
				since we get NACK on some if we ever
				connected to port 139 (the NACK is
				since we do not begin with RFC1001
				session initialize frame) */
				server->addr.sockAddr.sin_port =
					htons(CIFS_PORT);
				cifs_reconnect(server);
				csocket = server->ssocket;
				wake_up(&server->response_q);
				continue;
			}
		} else if (temp != (char) 0) {
			cERROR(1, "Unknown RFC 1002 frame");
			cifs_dump_mem(" Received Data: ", (char *)smb_buffer,
				      length);
			cifs_reconnect(server);
			csocket = server->ssocket;
			continue;
		}

		/* else we have an SMB response */
		if ((pdu_length > CIFSMaxBufSize + MAX_CIFS_HDR_SIZE - 4) ||
			    (pdu_length < sizeof(struct smb_hdr) - 1 - 4)) {
			cERROR(1, "Invalid size SMB length %d pdu_length %d",
					length, pdu_length+4);
			cifs_reconnect(server);
			csocket = server->ssocket;
			wake_up(&server->response_q);
			continue;
		}

		/* else length ok */
		reconnect = 0;

		if (pdu_length > MAX_CIFS_SMALL_BUFFER_SIZE - 4) {
			isLargeBuf = true;
			memcpy(bigbuf, smallbuf, 4);
			smb_buffer = bigbuf;
		}
		length = 0;
		iov.iov_base = 4 + (char *)smb_buffer;
		iov.iov_len = pdu_length;
		for (total_read = 0; total_read < pdu_length;
		     total_read += length) {
			length = kernel_recvmsg(csocket, &smb_msg, &iov, 1,
						pdu_length - total_read, 0);
			if ((server->tcpStatus == CifsExiting) ||
			    (length == -EINTR)) {
				/* then will exit */
				reconnect = 2;
				break;
			} else if (server->tcpStatus == CifsNeedReconnect) {
				cifs_reconnect(server);
				csocket = server->ssocket;
				/* Reconnect wakes up rspns q */
				/* Now we will reread sock */
				reconnect = 1;
				break;
			} else if ((length == -ERESTARTSYS) ||
				   (length == -EAGAIN)) {
				msleep(1); /* minimum sleep to prevent looping,
					      allowing socket to clear and app
					      threads to set tcpStatus
					      CifsNeedReconnect if server hung*/
				length = 0;
				continue;
			} else if (length <= 0) {
				cERROR(1, "Received no data, expecting %d",
					      pdu_length - total_read);
				cifs_reconnect(server);
				csocket = server->ssocket;
				reconnect = 1;
				break;
			}
		}
		if (reconnect == 2)
			break;
		else if (reconnect == 1)
			continue;

		length += 4; /* account for rfc1002 hdr */


		dump_smb(smb_buffer, length);
		if (checkSMB(smb_buffer, smb_buffer->Mid, total_read+4)) {
			cifs_dump_mem("Bad SMB: ", smb_buffer, 48);
			continue;
		}


		task_to_wake = NULL;
		spin_lock(&GlobalMid_Lock);
		list_for_each(tmp, &server->pending_mid_q) {
			mid_entry = list_entry(tmp, struct mid_q_entry, qhead);

			if ((mid_entry->mid == smb_buffer->Mid) &&
			    (mid_entry->midState == MID_REQUEST_SUBMITTED) &&
			    (mid_entry->command == smb_buffer->Command)) {
				if (check2ndT2(smb_buffer,server->maxBuf) > 0) {
					/* We have a multipart transact2 resp */
					isMultiRsp = true;
					if (mid_entry->resp_buf) {
						/* merge response - fix up 1st*/
						if (coalesce_t2(smb_buffer,
							mid_entry->resp_buf)) {
							mid_entry->multiRsp =
								 true;
							break;
						} else {
							/* all parts received */
							mid_entry->multiEnd =
								 true;
							goto multi_t2_fnd;
						}
					} else {
						if (!isLargeBuf) {
							cERROR(1, "1st trans2 resp needs bigbuf");
					/* BB maybe we can fix this up,  switch
					   to already allocated large buffer? */
						} else {
							/* Have first buffer */
							mid_entry->resp_buf =
								 smb_buffer;
							mid_entry->largeBuf =
								 true;
							bigbuf = NULL;
						}
					}
					break;
				}
				mid_entry->resp_buf = smb_buffer;
				mid_entry->largeBuf = isLargeBuf;
multi_t2_fnd:
				task_to_wake = mid_entry->tsk;
				mid_entry->midState = MID_RESPONSE_RECEIVED;
#ifdef CONFIG_CIFS_STATS2
				mid_entry->when_received = jiffies;
#endif
				/* so we do not time out requests to  server
				which is still responding (since server could
				be busy but not dead) */
				server->lstrp = jiffies;
				break;
			}
		}
		spin_unlock(&GlobalMid_Lock);
		if (task_to_wake) {
			/* Was previous buf put in mpx struct for multi-rsp? */
			if (!isMultiRsp) {
				/* smb buffer will be freed by user thread */
				if (isLargeBuf)
					bigbuf = NULL;
				else
					smallbuf = NULL;
			}
			wake_up_process(task_to_wake);
		} else if (!is_valid_oplock_break(smb_buffer, server) &&
			   !isMultiRsp) {
			cERROR(1, "No task to wake, unknown frame received! "
				   "NumMids %d", midCount.counter);
			cifs_dump_mem("Received Data is: ", (char *)smb_buffer,
				      sizeof(struct smb_hdr));
#ifdef CONFIG_CIFS_DEBUG2
			cifs_dump_detail(smb_buffer);
			cifs_dump_mids(server);
#endif /* CIFS_DEBUG2 */

		}
	} /* end while !EXITING */

	/* take it off the list, if it's not already */
	write_lock(&cifs_tcp_ses_lock);
	list_del_init(&server->tcp_ses_list);
	write_unlock(&cifs_tcp_ses_lock);

	spin_lock(&GlobalMid_Lock);
	server->tcpStatus = CifsExiting;
	spin_unlock(&GlobalMid_Lock);
	wake_up_all(&server->response_q);

	/* check if we have blocked requests that need to free */
	/* Note that cifs_max_pending is normally 50, but
	can be set at module install time to as little as two */
	spin_lock(&GlobalMid_Lock);
	if (atomic_read(&server->inFlight) >= cifs_max_pending)
		atomic_set(&server->inFlight, cifs_max_pending - 1);
	/* We do not want to set the max_pending too low or we
	could end up with the counter going negative */
	spin_unlock(&GlobalMid_Lock);
	/* Although there should not be any requests blocked on
	this queue it can not hurt to be paranoid and try to wake up requests
	that may haven been blocked when more than 50 at time were on the wire
	to the same server - they now will see the session is in exit state
	and get out of SendReceive.  */
	wake_up_all(&server->request_q);
	/* give those requests time to exit */
	msleep(125);

	if (server->ssocket) {
		sock_release(csocket);
		server->ssocket = NULL;
	}
	/* buffer usuallly freed in free_mid - need to free it here on exit */
	cifs_buf_release(bigbuf);
	if (smallbuf) /* no sense logging a debug message if NULL */
		cifs_small_buf_release(smallbuf);

	/*
	 * BB: we shouldn't have to do any of this. It shouldn't be
	 * possible to exit from the thread with active SMB sessions
	 */
	read_lock(&cifs_tcp_ses_lock);
	if (list_empty(&server->pending_mid_q)) {
		/* loop through server session structures attached to this and
		    mark them dead */
		list_for_each(tmp, &server->smb_ses_list) {
			ses = list_entry(tmp, struct cifsSesInfo,
					 smb_ses_list);
			ses->status = CifsExiting;
			ses->server = NULL;
		}
		read_unlock(&cifs_tcp_ses_lock);
	} else {
		/* although we can not zero the server struct pointer yet,
		since there are active requests which may depnd on them,
		mark the corresponding SMB sessions as exiting too */
		list_for_each(tmp, &server->smb_ses_list) {
			ses = list_entry(tmp, struct cifsSesInfo,
					 smb_ses_list);
			ses->status = CifsExiting;
		}

		spin_lock(&GlobalMid_Lock);
		list_for_each(tmp, &server->pending_mid_q) {
		mid_entry = list_entry(tmp, struct mid_q_entry, qhead);
			if (mid_entry->midState == MID_REQUEST_SUBMITTED) {
				cFYI(1, "Clearing Mid 0x%x - waking up ",
					 mid_entry->mid);
				task_to_wake = mid_entry->tsk;
				if (task_to_wake)
					wake_up_process(task_to_wake);
			}
		}
		spin_unlock(&GlobalMid_Lock);
		read_unlock(&cifs_tcp_ses_lock);
		/* 1/8th of sec is more than enough time for them to exit */
		msleep(125);
	}

	if (!list_empty(&server->pending_mid_q)) {
		/* mpx threads have not exited yet give them
		at least the smb send timeout time for long ops */
		/* due to delays on oplock break requests, we need
		to wait at least 45 seconds before giving up
		on a request getting a response and going ahead
		and killing cifsd */
		cFYI(1, "Wait for exit from demultiplex thread");
		msleep(46000);
		/* if threads still have not exited they are probably never
		coming home not much else we can do but free the memory */
	}

	/* last chance to mark ses pointers invalid
	if there are any pointing to this (e.g
	if a crazy root user tried to kill cifsd
	kernel thread explicitly this might happen) */
	/* BB: This shouldn't be necessary, see above */
	read_lock(&cifs_tcp_ses_lock);
	list_for_each(tmp, &server->smb_ses_list) {
		ses = list_entry(tmp, struct cifsSesInfo, smb_ses_list);
		ses->server = NULL;
	}
	read_unlock(&cifs_tcp_ses_lock);

	kfree(server->hostname);
	task_to_wake = xchg(&server->tsk, NULL);
	kfree(server);

	length = atomic_dec_return(&tcpSesAllocCount);
	if (length  > 0)
		mempool_resize(cifs_req_poolp, length + cifs_min_rcv,
				GFP_KERNEL);

	/* if server->tsk was NULL then wait for a signal before exiting */
	if (!task_to_wake) {
		set_current_state(TASK_INTERRUPTIBLE);
		while (!signal_pending(current)) {
			schedule();
			set_current_state(TASK_INTERRUPTIBLE);
		}
		set_current_state(TASK_RUNNING);
	}

	module_put_and_exit(0);
}
