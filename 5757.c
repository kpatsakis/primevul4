bool PamAuthenticateUser(BareosSocket* UA_sock,
                         const std::string& username_in,
                         const std::string& password_in,
                         std::string& authenticated_username)
{
  std::unique_ptr<PamData> pam_callback_data(new PamData(UA_sock, password_in));
  std::unique_ptr<struct pam_conv> pam_conversation_container(
      new struct pam_conv);
  struct pam_handle* pamh = nullptr; /* pam session handle */

  bool interactive = true;
  if (!username_in.empty() && !password_in.empty()) { interactive = false; }
  pam_conversation_container->conv
      = interactive ? PamConversationCallback : PamLocalCallback;
  pam_conversation_container->appdata_ptr = pam_callback_data.get();

  const char* username = username_in.empty() ? nullptr : username_in.c_str();
  int err = pam_start(service_name.c_str(), username,
                      pam_conversation_container.get(), &pamh);
  if (err != PAM_SUCCESS) {
    Dmsg1(debuglevel, "PAM start failed: %s\n", pam_strerror(pamh, err));
    return false;
  }

  err = DoPamAuth(pamh, username, authenticated_username);

  if (pam_end(pamh, err) != PAM_SUCCESS) {
    Dmsg1(debuglevel, "PAM end failed: %s\n", pam_strerror(pamh, err));
    return false;
  }

  if (err == PAM_SUCCESS) {
    bool ok = true;
    if (interactive) { ok = PamConvSendMessage(UA_sock, "", PAM_SUCCESS); }
    return ok;
  }
  return false;
}