int send_xmessage_using_uids(struct proclistlist *pll, char *message){
  int num_users;
  int lokke;
  int *uids=get_userlist(pll,&num_users);
  for(lokke=0;lokke<num_users;lokke++){
    char xauthpath[5000];
    struct passwd *pass=getpwuid(uids[lokke]);
    sprintf(xauthpath,"%s/.Xauthority",pass->pw_dir);
    if(send_xmessage(xauthpath,message)==1){
      free(uids);
      return 1;
    }
  }
  
  free(uids);
  return 0;
}
