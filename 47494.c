static int send_xmessage(char *xa_filename,char *message){
  if(access(xa_filename,R_OK)==0){
    setenv("XAUTHORITY",xa_filename,1);
    if(verbose)
      print_error(stdout,"Trying xauth file \"%s\"",xa_filename);
    if(system(message)==0)
      return 1;
  }
  return 0;
}
