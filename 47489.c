struct das_proclist *get_proclist(int *num_procs){
  int lokke=0;

  glibtop_proclist proclist_def={0};
  pid_t *proclist=glibtop_get_proclist(&proclist_def,GLIBTOP_KERN_PROC_ALL,0); //|GLIBTOP_EXCLUDE_SYSTEM,0);
  struct das_proclist *ret=calloc(sizeof(struct das_proclist),proclist_def.number);

  *num_procs=proclist_def.number;

  for(lokke=0;lokke<proclist_def.number;lokke++){
    pid_t pid=proclist[lokke];
    ret[lokke].pid=pid;
    ret[lokke].policy=sched_getscheduler(pid);
    ret[lokke].priority=get_pid_priority(pid);
    ret[lokke].start_time=get_pid_start_time(pid);
  }

#if LIBGTOP_MAJOR_VERSION<2
  glibtop_free(proclist);
#else
  g_free(proclist);
#endif

  return ret;
}
