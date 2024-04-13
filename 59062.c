void ef_debug(u_char level, const char *message, ...)
{ 
   va_list ap;
   
   /* if not in debug don't print anything */
   if (EF_GBL_OPTIONS->debug < level)
      return;

   /* print the message */ 
   va_start(ap, message);
   vfprintf (stderr, message, ap);
   fflush(stderr);
   va_end(ap);
   
}
