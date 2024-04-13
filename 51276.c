static char * nss_get_password(PK11SlotInfo * slot, PRBool retry, void *arg)
{
  (void)slot; /* unused */

  if(retry || NULL == arg)
    return NULL;
  else
    return (char *)PORT_Strdup((char *)arg);
}
