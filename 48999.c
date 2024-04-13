void glib_message_syslog_redirect(const gchar *log_domain,
                                  GLogLevelFlags log_level,
                                  const gchar *message,
                                  gpointer user_data)
{
    int level=LOG_DEBUG;
    
    switch( log_level )
    {
      case G_LOG_FLAG_FATAL:
      case G_LOG_LEVEL_CRITICAL:
      case G_LOG_LEVEL_ERROR:    
        level=LOG_ERR; 
        break;
      case G_LOG_LEVEL_WARNING:
        level=LOG_WARNING;
        break;
      case G_LOG_LEVEL_MESSAGE:
      case G_LOG_LEVEL_INFO:
        level=LOG_INFO;
        break;
      case G_LOG_LEVEL_DEBUG:
        level=LOG_DEBUG;
	break;
      default:
        level=LOG_ERR;
    }
    syslog(level, "%s", message);
}
