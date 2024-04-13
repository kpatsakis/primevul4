static bool PamConvSendMessage(BareosSocket* UA_sock,
                               const char* msg,
                               int msg_style)
{
  char buf = msg_style;
  if (!UA_sock->send((const char*)&buf, 1)) {
    Dmsg0(debuglevel, "PamConvSendMessage error\n");
    return false;
  }
  if (!UA_sock->send(msg, strlen(msg) + 1)) {
    Dmsg0(debuglevel, "PamConvSendMessage error\n");
    return false;
  }
  return true;
}