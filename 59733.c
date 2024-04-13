static int build_sec_desc(struct cifs_ntsd *pntsd, struct cifs_ntsd *pnntsd,
	__u32 secdesclen, __u64 nmode, kuid_t uid, kgid_t gid, int *aclflag)
{
	int rc = 0;
	__u32 dacloffset;
	__u32 ndacloffset;
	__u32 sidsoffset;
	struct cifs_sid *owner_sid_ptr, *group_sid_ptr;
	struct cifs_sid *nowner_sid_ptr, *ngroup_sid_ptr;
	struct cifs_acl *dacl_ptr = NULL;  /* no need for SACL ptr */
	struct cifs_acl *ndacl_ptr = NULL; /* no need for SACL ptr */

	if (nmode != NO_CHANGE_64) { /* chmod */
		owner_sid_ptr = (struct cifs_sid *)((char *)pntsd +
				le32_to_cpu(pntsd->osidoffset));
		group_sid_ptr = (struct cifs_sid *)((char *)pntsd +
				le32_to_cpu(pntsd->gsidoffset));
		dacloffset = le32_to_cpu(pntsd->dacloffset);
		dacl_ptr = (struct cifs_acl *)((char *)pntsd + dacloffset);
		ndacloffset = sizeof(struct cifs_ntsd);
		ndacl_ptr = (struct cifs_acl *)((char *)pnntsd + ndacloffset);
		ndacl_ptr->revision = dacl_ptr->revision;
		ndacl_ptr->size = 0;
		ndacl_ptr->num_aces = 0;

		rc = set_chmod_dacl(ndacl_ptr, owner_sid_ptr, group_sid_ptr,
					nmode);
		sidsoffset = ndacloffset + le16_to_cpu(ndacl_ptr->size);
		/* copy sec desc control portion & owner and group sids */
		copy_sec_desc(pntsd, pnntsd, sidsoffset);
		*aclflag = CIFS_ACL_DACL;
	} else {
		memcpy(pnntsd, pntsd, secdesclen);
		if (uid_valid(uid)) { /* chown */
			uid_t id;
			owner_sid_ptr = (struct cifs_sid *)((char *)pnntsd +
					le32_to_cpu(pnntsd->osidoffset));
			nowner_sid_ptr = kmalloc(sizeof(struct cifs_sid),
								GFP_KERNEL);
			if (!nowner_sid_ptr)
				return -ENOMEM;
			id = from_kuid(&init_user_ns, uid);
			rc = id_to_sid(id, SIDOWNER, nowner_sid_ptr);
			if (rc) {
				cifs_dbg(FYI, "%s: Mapping error %d for owner id %d\n",
					 __func__, rc, id);
				kfree(nowner_sid_ptr);
				return rc;
			}
			cifs_copy_sid(owner_sid_ptr, nowner_sid_ptr);
			kfree(nowner_sid_ptr);
			*aclflag = CIFS_ACL_OWNER;
		}
		if (gid_valid(gid)) { /* chgrp */
			gid_t id;
			group_sid_ptr = (struct cifs_sid *)((char *)pnntsd +
					le32_to_cpu(pnntsd->gsidoffset));
			ngroup_sid_ptr = kmalloc(sizeof(struct cifs_sid),
								GFP_KERNEL);
			if (!ngroup_sid_ptr)
				return -ENOMEM;
			id = from_kgid(&init_user_ns, gid);
			rc = id_to_sid(id, SIDGROUP, ngroup_sid_ptr);
			if (rc) {
				cifs_dbg(FYI, "%s: Mapping error %d for group id %d\n",
					 __func__, rc, id);
				kfree(ngroup_sid_ptr);
				return rc;
			}
			cifs_copy_sid(group_sid_ptr, ngroup_sid_ptr);
			kfree(ngroup_sid_ptr);
			*aclflag = CIFS_ACL_GROUP;
		}
	}

	return rc;
}
