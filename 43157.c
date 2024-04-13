smb2_hdr_assemble(struct smb2_hdr *hdr, __le16 smb2_cmd /* command */ ,
		  const struct cifs_tcon *tcon)
{
	struct smb2_pdu *pdu = (struct smb2_pdu *)hdr;
	char *temp = (char *)hdr;
	/* lookup word count ie StructureSize from table */
	__u16 parmsize = smb2_req_struct_sizes[le16_to_cpu(smb2_cmd)];

	/*
	 * smaller than SMALL_BUFFER_SIZE but bigger than fixed area of
	 * largest operations (Create)
	 */
	memset(temp, 0, 256);

	/* Note this is only network field converted to big endian */
	hdr->smb2_buf_length = cpu_to_be32(parmsize + sizeof(struct smb2_hdr)
			- 4 /*  RFC 1001 length field itself not counted */);

	hdr->ProtocolId[0] = 0xFE;
	hdr->ProtocolId[1] = 'S';
	hdr->ProtocolId[2] = 'M';
	hdr->ProtocolId[3] = 'B';
	hdr->StructureSize = cpu_to_le16(64);
	hdr->Command = smb2_cmd;
	hdr->CreditRequest = cpu_to_le16(2); /* BB make this dynamic */
	hdr->ProcessId = cpu_to_le32((__u16)current->tgid);

	if (!tcon)
		goto out;

	/* GLOBAL_CAP_LARGE_MTU will only be set if dialect > SMB2.02 */
	/* See sections 2.2.4 and 3.2.4.1.5 of MS-SMB2 */
	if ((tcon->ses) &&
	    (tcon->ses->server->capabilities & SMB2_GLOBAL_CAP_LARGE_MTU))
		hdr->CreditCharge = cpu_to_le16(1);
	/* else CreditCharge MBZ */

	hdr->TreeId = tcon->tid;
	/* Uid is not converted */
	if (tcon->ses)
		hdr->SessionId = tcon->ses->Suid;

	/*
	 * If we would set SMB2_FLAGS_DFS_OPERATIONS on open we also would have
	 * to pass the path on the Open SMB prefixed by \\server\share.
	 * Not sure when we would need to do the augmented path (if ever) and
	 * setting this flag breaks the SMB2 open operation since it is
	 * illegal to send an empty path name (without \\server\share prefix)
	 * when the DFS flag is set in the SMB open header. We could
	 * consider setting the flag on all operations other than open
	 * but it is safer to net set it for now.
	 */
/*	if (tcon->share_flags & SHI1005_FLAGS_DFS)
		hdr->Flags |= SMB2_FLAGS_DFS_OPERATIONS; */

	if (tcon->ses && tcon->ses->server && tcon->ses->server->sign)
		hdr->Flags |= SMB2_FLAGS_SIGNED;
out:
	pdu->StructureSize2 = cpu_to_le16(parmsize);
	return;
}
