static void nss_destroy_crl_item(void *user, void *ptr)
{
  SECItem *crl_der = (SECItem *)ptr;
  (void) user;
  SECITEM_FreeItem(crl_der, PR_TRUE);
}
