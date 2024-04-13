juniper_read_tlv_value(const u_char *p, u_int tlv_type, u_int tlv_len)
{
   int tlv_value;

   /* TLVs < 128 are little endian encoded */
   if (tlv_type < 128) {
       switch (tlv_len) {
       case 1:
           tlv_value = *p;
           break;
       case 2:
           tlv_value = EXTRACT_LE_16BITS(p);
           break;
       case 3:
           tlv_value = EXTRACT_LE_24BITS(p);
           break;
       case 4:
           tlv_value = EXTRACT_LE_32BITS(p);
           break;
       default:
           tlv_value = -1;
           break;
       }
   } else {
       /* TLVs >= 128 are big endian encoded */
       switch (tlv_len) {
       case 1:
           tlv_value = *p;
           break;
       case 2:
           tlv_value = EXTRACT_16BITS(p);
           break;
       case 3:
           tlv_value = EXTRACT_24BITS(p);
           break;
       case 4:
           tlv_value = EXTRACT_32BITS(p);
           break;
       default:
           tlv_value = -1;
           break;
       }
   }
   return tlv_value;
}
