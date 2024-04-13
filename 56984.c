check_options( void )
    {
#if defined(TILDE_MAP_1) && defined(TILDE_MAP_2)
    syslog( LOG_CRIT, "both TILDE_MAP_1 and TILDE_MAP_2 are defined" );
    exit( 1 );
#endif /* both */
    }
