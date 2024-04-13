static int PamLocalCallback(int num_msg,
#if defined(__sun)
                            struct pam_message** msgm,
#else
                            const struct pam_message** msgm,
#endif
                            struct pam_response** response,
                            void* appdata_ptr)
{
  struct pam_response* resp = static_cast<pam_response*>(
      calloc(num_msg, sizeof(struct pam_response)));

  PamData* pam_data = static_cast<PamData*>(appdata_ptr);

  if (num_msg == 1) {
    resp[0].resp = strdup(pam_data->passwd_.c_str());
    resp[0].resp_retcode = 0;
  }

  *response = resp;
  return PAM_SUCCESS;
}