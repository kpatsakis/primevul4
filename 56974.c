atoll( const char* str )
    {
    long long value;
    long long sign;

    while ( isspace( *str ) )
	++str;
    switch ( *str )
	{
	case '-': sign = -1; ++str; break;
	case '+': sign = 1; ++str; break;
	default: sign = 1; break;
	}
    value = 0;
    while ( isdigit( *str ) )
	{
	value = value * 10 + ( *str - '0' );
	++str;
	}
    return sign * value;
    }
