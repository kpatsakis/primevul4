load_cas_and_crls_dir(krb5_context context,
                      pkinit_plg_crypto_context plg_cryptoctx,
                      pkinit_req_crypto_context req_cryptoctx,
                      pkinit_identity_crypto_context id_cryptoctx,
                      int catype,
                      char *dirname)
{
    krb5_error_code retval = EINVAL;
    DIR *d = NULL;
    struct dirent *dentry = NULL;
    char filename[1024];

    if (dirname == NULL)
        return EINVAL;

    d = opendir(dirname);
    if (d == NULL)
        return ENOENT;

    while ((dentry = readdir(d))) {
        if (strlen(dirname) + strlen(dentry->d_name) + 2 > sizeof(filename)) {
            pkiDebug("%s: Path too long -- directory '%s' and file '%s'\n",
                     __FUNCTION__, dirname, dentry->d_name);
            goto cleanup;
        }
        /* Ignore subdirectories and anything starting with a dot */
#ifdef DT_DIR
        if (dentry->d_type == DT_DIR)
            continue;
#endif
        if (dentry->d_name[0] == '.')
            continue;
        snprintf(filename, sizeof(filename), "%s/%s", dirname, dentry->d_name);

        retval = load_cas_and_crls(context, plg_cryptoctx, req_cryptoctx,
                                   id_cryptoctx, catype, filename);
        if (retval)
            goto cleanup;
    }

    retval = 0;

cleanup:
    if (d != NULL)
        closedir(d);

    return retval;
}
