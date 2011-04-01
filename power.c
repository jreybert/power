

static void run_command (char *const *cmd, RESUSE *resp) {
  pid_t pid;			/* Pid of child.  */
  sighandler interrupt_signal, quit_signal;

  resuse_start (resp);

  pid = fork ();		/* Run CMD as child process.  */
  if (pid < 0)
    error (1, errno, "cannot fork");
  else if (pid == 0)
    {				/* If child.  */
      /* Don't cast execvp arguments; that causes errors on some systems,
	 versus merely warnings if the cast is left off.  */
      execvp (cmd[0], cmd);
      error (0, errno, "cannot run %s", cmd[0]);
      _exit (errno == ENOENT ? 127 : 126);
    }

  /* Have signals kill the child but not self (if possible).  */
  interrupt_signal = signal (SIGINT, SIG_IGN);
  quit_signal = signal (SIGQUIT, SIG_IGN);

  if (resuse_end (pid, resp) == 0)
    error (1, errno, "error waiting for child process");

  /* Re-enable signals.  */
  signal (SIGINT, interrupt_signal);
  signal (SIGQUIT, quit_signal);
}


void main (int argc, char **argv) {
  const char **command_line;

//  command_line = getargs (argc, argv);
//  run_command (command_line, &res);
//  summarize (outfp, output_format, command_line, &res);
//  fflush (outfp);

  if (WIFSTOPPED (res.waitstatus))
    exit (WSTOPSIG (res.waitstatus) + 128);
  else if (WIFSIGNALED (res.waitstatus))
    exit (WTERMSIG (res.waitstatus) + 128);
  else if (WIFEXITED (res.waitstatus))
    exit (WEXITSTATUS (res.waitstatus));
}

