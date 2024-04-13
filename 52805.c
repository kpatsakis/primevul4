int print_encoded_msg(FILE* fd,char *code,char *prefix)
{
   unsigned short int i,j,k,l,m,msglen;
   char r,*msg;
   unsigned char *payload;
   
   payload=(unsigned char*)code;
   memcpy(&i,code,2);
   memcpy(&j,&code[MSG_START_IDX],2);
   memcpy(&msglen,&code[MSG_LEN_IDX],2);
   i=ntohs(i);
   j=ntohs(j);
   msglen=ntohs(msglen);
   for(k=0;k<j;k++)
      fprintf(fd,"%s%d%s",k==0?"ENCODED-MSG:[":":",payload[k],k==j-1?"]\n":"");
   msg=(char*)&payload[j];
   fprintf(fd,"MESSAGE:\n[%.*s]\n",msglen,msg);
   r=(i<100)?1:0;
   if(r){
      fprintf(fd,"%sREQUEST CODE=%d==%.*s,URI=%.*s,VERSION=%*.s\n",prefix,i,
	    payload[METHOD_CODE_IDX+1],&msg[payload[METHOD_CODE_IDX]],
	    payload[URI_REASON_IDX+1],&msg[payload[URI_REASON_IDX]],
	    payload[VERSION_IDX+1],&msg[payload[VERSION_IDX]]);
      print_encoded_uri(fd,&payload[REQUEST_URI_IDX+1],payload[REQUEST_URI_IDX],msg,50,strcat(prefix,"  "));
      prefix[strlen(prefix)-2]=0;
      i=REQUEST_URI_IDX+1+payload[REQUEST_URI_IDX];
   }else{
      fprintf(fd,"%sRESPONSE CODE=%d==%.*s,REASON=%.*s,VERSION=%.*s\n",prefix,i,
	    payload[METHOD_CODE_IDX+1],&msg[payload[METHOD_CODE_IDX]],
	    payload[URI_REASON_IDX+1],&msg[payload[URI_REASON_IDX]],
	    payload[VERSION_IDX+1],&msg[payload[VERSION_IDX]]);
      i=REQUEST_URI_IDX;
   }
   k=((payload[CONTENT_IDX]<<8)|payload[CONTENT_IDX+1]);
   j=msglen-k;
   fprintf(fd,"%sMESSAGE CONTENT:%.*s\n",prefix,j,&msg[k]);
   j=payload[i];
   fprintf(fd,"%sHEADERS PRESENT(%d):",prefix,j);
   i++;
   for(k=i;k<i+(j*3);k+=3)
      fprintf(fd,"%c%d%c",k==i?'[':',',payload[k],k==(i+3*j-3)?']':' ');
   fprintf(fd,"\n");
   for(k=i;k<i+(j*3);k+=3){
      memcpy(&l,&payload[k+1],2);
      memcpy(&m,&payload[k+4],2);
      l=ntohs(l);
      m=ntohs(m);
      print_encoded_header(fd,msg,msglen,&payload[l],m-l,payload[k],prefix);
   }
   return 1;
}
