int decode_msg(struct sip_msg *msg,char *code, unsigned int len)
{
   unsigned short int h;
   char *myerror=NULL;

   memcpy(&h,&code[2],2);
   h=ntohs(h);
   /*TODO use shorcuts in meta-info header.*/

   msg->buf=&code[h];
   memcpy(&h,&code[4],2);
   h=ntohs(h);
   msg->len=h;
   if(parse_headers(msg,HDR_EOH_F,0)<0){
      myerror="in parse_headers";
      goto error;
   }
error:
   LM_ERR("(%s)\n",myerror);
   return -1;
}
