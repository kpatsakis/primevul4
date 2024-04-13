void CLASS merror (void *ptr, const char *where)
{
  if (ptr) return;
  dcraw_message (DCRAW_ERROR,_("%s: Out of memory in %s\n"), ifname_display, where);
  longjmp (failure, 1);
}
