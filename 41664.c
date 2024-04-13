SCTP_STATIC int sctp_msghdr_parse(const struct msghdr *msg,
				  sctp_cmsgs_t *cmsgs)
{
	struct cmsghdr *cmsg;
	struct msghdr *my_msg = (struct msghdr *)msg;

	for (cmsg = CMSG_FIRSTHDR(msg);
	     cmsg != NULL;
	     cmsg = CMSG_NXTHDR(my_msg, cmsg)) {
		if (!CMSG_OK(my_msg, cmsg))
			return -EINVAL;

		/* Should we parse this header or ignore?  */
		if (cmsg->cmsg_level != IPPROTO_SCTP)
			continue;

		/* Strictly check lengths following example in SCM code.  */
		switch (cmsg->cmsg_type) {
		case SCTP_INIT:
			/* SCTP Socket API Extension
			 * 5.2.1 SCTP Initiation Structure (SCTP_INIT)
			 *
			 * This cmsghdr structure provides information for
			 * initializing new SCTP associations with sendmsg().
			 * The SCTP_INITMSG socket option uses this same data
			 * structure.  This structure is not used for
			 * recvmsg().
			 *
			 * cmsg_level    cmsg_type      cmsg_data[]
			 * ------------  ------------   ----------------------
			 * IPPROTO_SCTP  SCTP_INIT      struct sctp_initmsg
			 */
			if (cmsg->cmsg_len !=
			    CMSG_LEN(sizeof(struct sctp_initmsg)))
				return -EINVAL;
			cmsgs->init = (struct sctp_initmsg *)CMSG_DATA(cmsg);
			break;

		case SCTP_SNDRCV:
			/* SCTP Socket API Extension
			 * 5.2.2 SCTP Header Information Structure(SCTP_SNDRCV)
			 *
			 * This cmsghdr structure specifies SCTP options for
			 * sendmsg() and describes SCTP header information
			 * about a received message through recvmsg().
			 *
			 * cmsg_level    cmsg_type      cmsg_data[]
			 * ------------  ------------   ----------------------
			 * IPPROTO_SCTP  SCTP_SNDRCV    struct sctp_sndrcvinfo
			 */
			if (cmsg->cmsg_len !=
			    CMSG_LEN(sizeof(struct sctp_sndrcvinfo)))
				return -EINVAL;

			cmsgs->info =
				(struct sctp_sndrcvinfo *)CMSG_DATA(cmsg);

			/* Minimally, validate the sinfo_flags. */
			if (cmsgs->info->sinfo_flags &
			    ~(SCTP_UNORDERED | SCTP_ADDR_OVER |
			      SCTP_ABORT | SCTP_EOF))
				return -EINVAL;
			break;

		default:
			return -EINVAL;
		}
	}
	return 0;
}
