static void print_error(FILE *where,char *fmt, ...) {
  char temp[10000];
  va_list ap;
  va_start(ap, fmt);{
    vsnprintf (temp, 9998, fmt, ap);
  }va_end(ap);
  syslog(LOG_INFO,temp);
  if(where!=NULL)
    fprintf(where,"Das_Watchdog: %s\n",temp);
}
