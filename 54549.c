make_etype_info_entry(krb5_context context, ETYPE_INFO_ENTRY *ent, Key *key)
{
    ent->etype = key->key.keytype;
    if(key->salt){
#if 0
	ALLOC(ent->salttype);

	if(key->salt->type == hdb_pw_salt)
	    *ent->salttype = 0; /* or 1? or NULL? */
	else if(key->salt->type == hdb_afs3_salt)
	    *ent->salttype = 2;
	else {
	    kdc_log(context, config, 0, "unknown salt-type: %d",
		    key->salt->type);
	    return KRB5KRB_ERR_GENERIC;
	}
	/* according to `the specs', we can't send a salt if
	   we have AFS3 salted key, but that requires that you
	   *know* what cell you are using (e.g by assuming
	   that the cell is the same as the realm in lower
	   case) */
#elif 0
	ALLOC(ent->salttype);
	*ent->salttype = key->salt->type;
#else
	/*
	 * We shouldn't sent salttype since it is incompatible with the
	 * specification and it breaks windows clients.  The afs
	 * salting problem is solved by using KRB5-PADATA-AFS3-SALT
	 * implemented in Heimdal 0.7 and later.
	 */
	ent->salttype = NULL;
#endif
	krb5_copy_data(context, &key->salt->salt,
		       &ent->salt);
    } else {
	/* we return no salt type at all, as that should indicate
	 * the default salt type and make everybody happy.  some
	 * systems (like w2k) dislike being told the salt type
	 * here. */

	ent->salttype = NULL;
	ent->salt = NULL;
    }
    return 0;
}
