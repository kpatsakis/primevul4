static CURLcode nss_load_crl(const char* crlfilename)
{
  PRFileDesc *infile;
  PRFileInfo  info;
  SECItem filedata = { 0, NULL, 0 };
  SECItem *crl_der = NULL;
  char *body;

  infile = PR_Open(crlfilename, PR_RDONLY, 0);
  if(!infile)
    return CURLE_SSL_CRL_BADFILE;

  if(PR_SUCCESS != PR_GetOpenFileInfo(infile, &info))
    goto fail;

  if(!SECITEM_AllocItem(NULL, &filedata, info.size + /* zero ended */ 1))
    goto fail;

  if(info.size != PR_Read(infile, filedata.data, info.size))
    goto fail;

  crl_der = SECITEM_AllocItem(NULL, NULL, 0U);
  if(!crl_der)
    goto fail;

  /* place a trailing zero right after the visible data */
  body = (char*)filedata.data;
  body[--filedata.len] = '\0';

  body = strstr(body, "-----BEGIN");
  if(body) {
    /* assume ASCII */
    char *trailer;
    char *begin = PORT_Strchr(body, '\n');
    if(!begin)
      begin = PORT_Strchr(body, '\r');
    if(!begin)
      goto fail;

    trailer = strstr(++begin, "-----END");
    if(!trailer)
      goto fail;

    /* retrieve DER from ASCII */
    *trailer = '\0';
    if(ATOB_ConvertAsciiToItem(crl_der, begin))
      goto fail;

    SECITEM_FreeItem(&filedata, PR_FALSE);
  }
  else
    /* assume DER */
    *crl_der = filedata;

  PR_Close(infile);
  return nss_cache_crl(crl_der);

fail:
  PR_Close(infile);
  SECITEM_FreeItem(crl_der, PR_TRUE);
  SECITEM_FreeItem(&filedata, PR_FALSE);
  return CURLE_SSL_CRL_BADFILE;
}
