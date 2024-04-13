static int checksoftirq2(int force,int cpu){
  char name[500];
  pid_t pid=gettimerpid(&name[0],cpu);

  if(pid==-1) return 0;


  {
    int policy=sched_getscheduler(pid);
    int priority=get_pid_priority(pid);

    if(priority<sched_get_priority_max(SCHED_FIFO)
       || policy==SCHED_OTHER
       )
      {

	if(force){
	  print_error(stdout,"Forcing %s to SCHED_FIFO priority %d",name,sched_get_priority_max(SCHED_FIFO));
	  set_pid_priority(pid,SCHED_FIFO,sched_get_priority_max(SCHED_FIFO),"Could not set %d (\"%s\") to SCHED_FIFO (%s).\n\n",name);
	  return checksoftirq2(0,cpu);
	}	  
	

	if(priority<sched_get_priority_max(SCHED_FIFO))
	  print_error(stderr,
		      "\n\nWarning. The priority of the \"%s\" process is only %d, and not %d. Unless you are using the High Res Timer,\n"
		      "the watchdog will probably not work. If you are using the High Res Timer, please continue doing so and ignore this message.\n",
		      name,
		      priority,
		      sched_get_priority_max(SCHED_FIFO)
		      );
	if(policy==SCHED_OTHER)
	  print_error(stderr,
		      "\n\nWarning The \"%s\" process is running SCHED_OTHER. Unless you are using the High Res Timer,\n"
		      "the watchdog will probably not work, and the timing on your machine is probably horrible.\n",
		      name
		      );
	
	if(checkirq){
	  print_error(stdout,"\n\nUnless you are using the High Res Timer, you need to add the \"--force\" flag to run das_watchdog reliably.\n");
	  print_error(stdout,"(Things might change though, so it could work despite all warnings above. To test the watchdog, run the \"test_rt\" program.)\n\n");
	}
	return -1;
      }
    
    return 1;
  }
}
