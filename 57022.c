sockaddr_check( httpd_sockaddr* saP )
    {
    switch ( saP->sa.sa_family )
	{
	case AF_INET: return 1;
#ifdef USE_IPV6
	case AF_INET6: return 1;
#endif /* USE_IPV6 */
	default:
	return 0;
	}
    }
