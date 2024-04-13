print_attr_num(netdissect_options *ndo,
               register const u_char *data, u_int length, u_short attr_code)
{
   uint32_t timeout;

   if (length != 4)
   {
       ND_PRINT((ndo, "ERROR: length %u != 4", length));
       return;
   }

   ND_TCHECK2(data[0],4);
                          /* This attribute has standard values */
   if (attr_type[attr_code].siz_subtypes)
   {
      static const char **table;
      uint32_t data_value;
      table = attr_type[attr_code].subtypes;

      if ( (attr_code == TUNNEL_TYPE) || (attr_code == TUNNEL_MEDIUM) )
      {
         if (!*data)
            ND_PRINT((ndo, "Tag[Unused] "));
         else
            ND_PRINT((ndo, "Tag[%d] ", *data));
         data++;
         data_value = EXTRACT_24BITS(data);
      }
      else
      {
         data_value = EXTRACT_32BITS(data);
      }
      if ( data_value <= (uint32_t)(attr_type[attr_code].siz_subtypes - 1 +
            attr_type[attr_code].first_subtype) &&
	   data_value >= attr_type[attr_code].first_subtype )
         ND_PRINT((ndo, "%s", table[data_value]));
      else
         ND_PRINT((ndo, "#%u", data_value));
   }
   else
   {
      switch(attr_code) /* Be aware of special cases... */
      {
        case FRM_IPX:
             if (EXTRACT_32BITS( data) == 0xFFFFFFFE )
                ND_PRINT((ndo, "NAS Select"));
             else
                ND_PRINT((ndo, "%d", EXTRACT_32BITS(data)));
          break;

        case SESSION_TIMEOUT:
        case IDLE_TIMEOUT:
        case ACCT_DELAY:
        case ACCT_SESSION_TIME:
        case ACCT_INT_INTERVAL:
             timeout = EXTRACT_32BITS( data);
             if ( timeout < 60 )
                ND_PRINT((ndo,  "%02d secs", timeout));
             else
             {
                if ( timeout < 3600 )
                   ND_PRINT((ndo,  "%02d:%02d min",
                          timeout / 60, timeout % 60));
                else
                   ND_PRINT((ndo, "%02d:%02d:%02d hours",
                          timeout / 3600, (timeout % 3600) / 60,
                          timeout % 60));
             }
          break;

        case FRM_ATALK_LINK:
             if (EXTRACT_32BITS(data) )
                ND_PRINT((ndo, "%d", EXTRACT_32BITS(data)));
             else
                ND_PRINT((ndo, "Unnumbered"));
          break;

        case FRM_ATALK_NETWORK:
             if (EXTRACT_32BITS(data) )
                ND_PRINT((ndo, "%d", EXTRACT_32BITS(data)));
             else
                ND_PRINT((ndo, "NAS assigned"));
          break;

        case TUNNEL_PREFERENCE:
            if (*data)
               ND_PRINT((ndo, "Tag[%d] ", *data));
            else
               ND_PRINT((ndo, "Tag[Unused] "));
            data++;
            ND_PRINT((ndo, "%d", EXTRACT_24BITS(data)));
          break;

        case EGRESS_VLAN_ID:
            ND_PRINT((ndo, "%s (0x%02x) ",
                   tok2str(rfc4675_tagged,"Unknown tag",*data),
                   *data));
            data++;
            ND_PRINT((ndo, "%d", EXTRACT_24BITS(data)));
          break;

        default:
             ND_PRINT((ndo, "%d", EXTRACT_32BITS(data)));
          break;

      } /* switch */

   } /* if-else */

   return;

   trunc:
     ND_PRINT((ndo, "%s", tstr));
}
