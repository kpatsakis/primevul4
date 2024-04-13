static int PamConversationCallback(int num_msg,
#if defined(__sun)
                                   struct pam_message** msgm,
#else
                                   const struct pam_message** msgm,
#endif
                                   struct pam_response** response,
                                   void* appdata_ptr)
{
  if (!appdata_ptr) {
    Dmsg0(debuglevel, "pam_conv_callback pointer error\n");
    return PAM_BUF_ERR;
  }

  if ((num_msg <= 0) || (num_msg > PAM_MAX_NUM_MSG)) {
    Dmsg0(debuglevel, "pam_conv_callback wrong number of messages\n");
    return (PAM_CONV_ERR);
  }

  struct pam_response* resp = static_cast<pam_response*>(
      calloc(num_msg, sizeof(struct pam_response)));

  if (!resp) {
    Dmsg0(debuglevel, "pam_conv_callback memory error\n");
    return PAM_BUF_ERR;
  }

  PamData* pam_data = static_cast<PamData*>(appdata_ptr);

  bool error = false;
  int i = 0;
  for (; i < num_msg && !error; i++) {
    switch (msgm[i]->msg_style) {
      case PAM_PROMPT_ECHO_OFF:
      case PAM_PROMPT_ECHO_ON:
        if (!PamConvSendMessage(pam_data->UA_sock_, msgm[i]->msg,
                                msgm[i]->msg_style)) {
          error = true;
          break;
        }
        if (pam_data->UA_sock_->IsStop() || pam_data->UA_sock_->IsError()) {
          error = true;
          break;
        }
        if (pam_data->UA_sock_->recv()) {
          resp[i].resp = strdup(pam_data->UA_sock_->msg);
          resp[i].resp_retcode = 0;
        }
        if (pam_data->UA_sock_->IsStop() || pam_data->UA_sock_->IsError()) {
          error = true;
          break;
        }
        break;
      case PAM_ERROR_MSG:
      case PAM_TEXT_INFO:
        if (!PamConvSendMessage(pam_data->UA_sock_, msgm[i]->msg,
                                PAM_PROMPT_ECHO_ON)) {
          error = true;
        }
        break;
      default:
        Dmsg3(debuglevel, "message[%d]: pam error type: %d error: \"%s\"\n", 1,
              msgm[i]->msg_style, msgm[i]->msg);
        error = true;
        break;
    } /* switch (msgm[i]->msg_style) { */
  }   /* for( ; i < num_msg ..) */

  if (error) {
    for (int i = 0; i < num_msg; ++i) {
      if (resp[i].resp) {
        memset(resp[i].resp, 0, strlen(resp[i].resp));
        free(resp[i].resp);
      }
    }
    memset(resp, 0, num_msg * sizeof *resp);
    free(resp);
    *response = nullptr;
    return PAM_CONV_ERR;
  }

  *response = resp;
  return PAM_SUCCESS;
}