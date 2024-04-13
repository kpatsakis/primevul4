radius_print(netdissect_options *ndo,
             const u_char *dat, u_int length)
{
   register const struct radius_hdr *rad;
   u_int len, auth_idx;

   ND_TCHECK2(*dat, MIN_RADIUS_LEN);
   rad = (const struct radius_hdr *)dat;
   len = EXTRACT_16BITS(&rad->len);

   if (len < MIN_RADIUS_LEN)
   {
	  ND_PRINT((ndo, "%s", tstr));
	  return;
   }

   if (len > length)
	  len = length;

   if (ndo->ndo_vflag < 1) {
       ND_PRINT((ndo, "RADIUS, %s (%u), id: 0x%02x length: %u",
              tok2str(radius_command_values,"Unknown Command",rad->code),
              rad->code,
              rad->id,
              len));
       return;
   }
   else {
       ND_PRINT((ndo, "RADIUS, length: %u\n\t%s (%u), id: 0x%02x, Authenticator: ",
              len,
              tok2str(radius_command_values,"Unknown Command",rad->code),
              rad->code,
              rad->id));

       for(auth_idx=0; auth_idx < 16; auth_idx++)
            ND_PRINT((ndo, "%02x", rad->auth[auth_idx]));
   }

   if (len > MIN_RADIUS_LEN)
      radius_attrs_print(ndo, dat + MIN_RADIUS_LEN, len - MIN_RADIUS_LEN);
   return;

trunc:
   ND_PRINT((ndo, "%s", tstr));
}
