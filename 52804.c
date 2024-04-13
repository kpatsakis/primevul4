int dump_msg_test(char *code,FILE* fd,char header,char segregationLevel)
{
   unsigned short int i,j,l,m,msglen;
   int k;
   char r,*msg;
   unsigned char *payload;
   payload=(unsigned char*)code;
   memcpy(&i,code,2);/*the CODE of the request/response*/
   memcpy(&j,&code[MSG_START_IDX],2);/*where the MSG starts*/
   memcpy(&msglen,&code[MSG_LEN_IDX],2);/*how long the MSG is*/
   i=ntohs(i);
   j=ntohs(j);
   msglen=ntohs(msglen);
   if(header==0){
      fwrite(code,1,j+msglen,fd);
      fwrite(&theSignal,1,4,fd);
      return 0;
   }
   msg=(char*)&payload[j];
   r=(i<100)?1:0;
   if(r){
      if(segregationLevel & ALSO_RURI){
	 if(!(segregationLevel & JUNIT)){ 
	    
	    k=htonl(payload[REQUEST_URI_IDX+1]+payload[REQUEST_URI_IDX+2]);
	    fwrite(&k,1,4,fd);
	    fwrite(msg,1,ntohl(k),fd);
	    k=htonl((long)payload[REQUEST_URI_IDX]);
	    fwrite(&k,1,4,fd);
	    fwrite(&payload[REQUEST_URI_IDX+1],1,payload[REQUEST_URI_IDX],fd);
	    fwrite(&theSignal,1,4,fd);
	 }else
	    print_uri_junit_tests(msg,payload[REQUEST_URI_IDX+1]+payload[REQUEST_URI_IDX+2]
		  ,&payload[REQUEST_URI_IDX+1],payload[REQUEST_URI_IDX],fd,1,"");
      }
      i=REQUEST_URI_IDX+1+payload[REQUEST_URI_IDX];
   }else{
      i=REQUEST_URI_IDX;
   }
   j=payload[i];
   i++;
   for(k=i;k<i+(j*3);k+=3){
      memcpy(&l,&payload[k+1],2);
      memcpy(&m,&payload[k+4],2);
      l=ntohs(l);
      m=ntohs(m);
      if(header==(char)payload[k] ||
	    (header=='U' &&
	     (payload[k]=='f' ||
	      payload[k]=='t' ||
	      payload[k]=='m' ||
	      payload[k]=='o' ||
	      payload[k]=='p')))
	 dump_headers_test(msg,msglen,&payload[i+(j*3)+l+3],m-l,payload[k],fd,segregationLevel);
   }
   return 1;
}
