print_attr_address(netdissect_options *ndo,
                   register const u_char *data, u_int length, u_short attr_code)
{
   if (length != 4)
   {
       ND_PRINT((ndo, "ERROR: length %u != 4", length));
       return;
   }

   ND_TCHECK2(data[0],4);

   switch(attr_code)
   {
      case FRM_IPADDR:
      case LOG_IPHOST:
           if (EXTRACT_32BITS(data) == 0xFFFFFFFF )
              ND_PRINT((ndo, "User Selected"));
           else
              if (EXTRACT_32BITS(data) == 0xFFFFFFFE )
                 ND_PRINT((ndo, "NAS Select"));
              else
                 ND_PRINT((ndo, "%s",ipaddr_string(ndo, data)));
      break;

      default:
          ND_PRINT((ndo, "%s", ipaddr_string(ndo, data)));
      break;
   }

   return;

   trunc:
     ND_PRINT((ndo, "%s", tstr));
}
