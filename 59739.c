static void copy_sec_desc(const struct cifs_ntsd *pntsd,
				struct cifs_ntsd *pnntsd, __u32 sidsoffset)
{
	struct cifs_sid *owner_sid_ptr, *group_sid_ptr;
	struct cifs_sid *nowner_sid_ptr, *ngroup_sid_ptr;

	/* copy security descriptor control portion */
	pnntsd->revision = pntsd->revision;
	pnntsd->type = pntsd->type;
	pnntsd->dacloffset = cpu_to_le32(sizeof(struct cifs_ntsd));
	pnntsd->sacloffset = 0;
	pnntsd->osidoffset = cpu_to_le32(sidsoffset);
	pnntsd->gsidoffset = cpu_to_le32(sidsoffset + sizeof(struct cifs_sid));

	/* copy owner sid */
	owner_sid_ptr = (struct cifs_sid *)((char *)pntsd +
				le32_to_cpu(pntsd->osidoffset));
	nowner_sid_ptr = (struct cifs_sid *)((char *)pnntsd + sidsoffset);
	cifs_copy_sid(nowner_sid_ptr, owner_sid_ptr);

	/* copy group sid */
	group_sid_ptr = (struct cifs_sid *)((char *)pntsd +
				le32_to_cpu(pntsd->gsidoffset));
	ngroup_sid_ptr = (struct cifs_sid *)((char *)pnntsd + sidsoffset +
					sizeof(struct cifs_sid));
	cifs_copy_sid(ngroup_sid_ptr, group_sid_ptr);

	return;
}
