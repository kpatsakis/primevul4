bsg_validate_sgv4_hdr(struct sg_io_v4 *hdr, int *rw)
{
	int ret = 0;

	if (hdr->guard != 'Q')
		return -EINVAL;

	switch (hdr->protocol) {
	case BSG_PROTOCOL_SCSI:
		switch (hdr->subprotocol) {
		case BSG_SUB_PROTOCOL_SCSI_CMD:
		case BSG_SUB_PROTOCOL_SCSI_TRANSPORT:
			break;
		default:
			ret = -EINVAL;
		}
		break;
	default:
		ret = -EINVAL;
	}

	*rw = hdr->dout_xfer_len ? WRITE : READ;
	return ret;
}
