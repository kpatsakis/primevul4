pkinit_get_certs_dir(krb5_context context,
                     pkinit_plg_crypto_context plg_cryptoctx,
                     pkinit_req_crypto_context req_cryptoctx,
                     pkinit_identity_opts *idopts,
                     pkinit_identity_crypto_context id_cryptoctx,
                     krb5_principal princ)
{
    krb5_error_code retval = ENOMEM;
    DIR *d = NULL;
    struct dirent *dentry = NULL;
    char certname[1024];
    char keyname[1024];
    int i = 0, len;
    char *dirname, *suf;

    if (idopts->cert_filename == NULL) {
        pkiDebug("%s: failed to get user's certificate directory location\n",
                 __FUNCTION__);
        return ENOENT;
    }

    dirname = idopts->cert_filename;
    d = opendir(dirname);
    if (d == NULL)
        return errno;

    /*
     * We'll assume that certs are named XXX.crt and the corresponding
     * key is named XXX.key
     */
    while ((i < MAX_CREDS_ALLOWED) &&  (dentry = readdir(d)) != NULL) {
        /* Ignore subdirectories and anything starting with a dot */
#ifdef DT_DIR
        if (dentry->d_type == DT_DIR)
            continue;
#endif
        if (dentry->d_name[0] == '.')
            continue;
        len = strlen(dentry->d_name);
        if (len < 5)
            continue;
        suf = dentry->d_name + (len - 4);
        if (strncmp(suf, ".crt", 4) != 0)
            continue;

        /* Checked length */
        if (strlen(dirname) + strlen(dentry->d_name) + 2 > sizeof(certname)) {
            pkiDebug("%s: Path too long -- directory '%s' and file '%s'\n",
                     __FUNCTION__, dirname, dentry->d_name);
            continue;
        }
        snprintf(certname, sizeof(certname), "%s/%s", dirname, dentry->d_name);
        snprintf(keyname, sizeof(keyname), "%s/%s", dirname, dentry->d_name);
        len = strlen(keyname);
        keyname[len - 3] = 'k';
        keyname[len - 2] = 'e';
        keyname[len - 1] = 'y';

        retval = pkinit_load_fs_cert_and_key(context, id_cryptoctx,
                                             certname, keyname, i);
        if (retval == 0) {
            pkiDebug("%s: Successfully loaded cert (and key) for %s\n",
                     __FUNCTION__, dentry->d_name);
            i++;
        }
        else
            continue;
    }

    if (i == 0) {
        pkiDebug("%s: No cert/key pairs found in directory '%s'\n",
                 __FUNCTION__, idopts->cert_filename);
        retval = ENOENT;
        goto cleanup;
    }

    retval = 0;

cleanup:
    if (d)
        closedir(d);

    return retval;
}
