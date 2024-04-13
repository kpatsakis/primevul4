static int set_pid_priority(pid_t pid,int policy,int priority,char *message,char *name){
  struct sched_param par={0};
  par.sched_priority=priority;
  if((sched_setscheduler(pid,policy,&par)!=0)){
    print_error(stderr,message,pid,name,strerror(errno));
    return 0;
  }
  return 1;
}
