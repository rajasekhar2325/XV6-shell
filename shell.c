#include "types.h"
#include "user.h"
#include "fcntl.h"

char *comm[10];
int totcmd = 0;
char *lcomm[5];
char *rcomm[5];
int cmdtype = 1;
char *inpfile;
char *outfile;
int piperdr = 0;
int pipewdr = 0;

void parseinp(char *inp)
{
  int j = 0;
  int k = 0;

  for (int i = 0; i < 10; i++)
    comm[i] = 0;
  for (int i = 0; i < 5; i++)
  {
    lcomm[i] = 0;
    rcomm[i] = 0;
  }

  comm[0] = malloc(sizeof(char) * 10);
  lcomm[0] = malloc(sizeof(char) * 10);
  rcomm[0] = malloc(sizeof(char) * 10);

  for (int i = 0; inp[i] != '\n'; i++)
  {

    if (inp[i] != ' ' && inp[i] != '|' && inp[i] != '&' && inp[i] != ';')
    {
      comm[j][k] = inp[i];
      k++;
    }
    else if (inp[i] == ' ' && (inp[i + 1] == ' ' || inp[i + 1] == '|' || inp[i + 1] == '&' || inp[i + 1] == ';'))
      continue;
    else if (inp[i] == ' ' && inp[i + 1] != ' ')
    {
      j++;
      comm[j] = malloc(sizeof(char) * 10);
      k = 0;
    }
    else if ((inp[i] == '|' || inp[i] == '&' || inp[i] == ';') && (inp[i + 1] == '|' || inp[i + 1] == '&' || inp[i + 1] == ';' || inp[i + 1] == ' '))
    {
      j++;
      comm[j] = malloc(sizeof(char) * 10);
      k = 0;
      comm[j][k] = inp[i];
      // j++;
      // comm[j]=malloc(sizeof(char)*10);
    }
    else if ((inp[i] == '|' || inp[i] == '&' || inp[i] == ';') && (inp[i + 1] != '|' || inp[i + 1] != '&' || inp[i + 1] != ';') && (inp[i + 1] != ' '))
    {
      j++;
      comm[j] = malloc(sizeof(char) * 10);
      k = 0;
      comm[j][k] = inp[i];
      j++;
      comm[j] = malloc(sizeof(char) * 10);
    }

    totcmd = j;
  }

  int reflag = 0;  // both <> flag
  int spcount = 0; // pipe along with <> flag

  for (int i = 0; i < 10; i++)
  {
    if (comm[i][0] == '|')
      spcount += 2;
    if (comm[i][0] == '<' || comm[i][0] == '>')
      spcount++;
  }

  for (int i = 0; i < 10; i++)
  {
    if (comm[i][0] == '|' && comm[i + 1][0] != '|' && comm[i - 1][0] != '|')
    {
      // printf(2,"ctype pipe\n");
      cmdtype = 2;
      lcomm[0] = comm[0];
      rcomm[0] = comm[i + 1];

      for (int j = 0; j < i; j++)
        lcomm[j] = comm[j];
      // for(int i=0;i<5;i++)
      //   printf(2,"lcomm: %s %d\n",lcomm[i],strlen(lcomm[i]));
      for (int i = 0; i < 5; i++)
      {
        if (lcomm[i][0] == '<')
        {
          piperdr = 1;
          inpfile = lcomm[i + 1];
          lcomm[i] = 0;
          lcomm[i + 1] = 0;
          break;
        }
      }
      for (int j = i + 1, k = 0; j <= totcmd; j++, k++)
        rcomm[k] = comm[j];
      // for(int i=0;i<5;i++)
      //   printf(2,"rcomm: %s %d\n",rcomm[i],strlen(rcomm[i]));
      for (int i = 0; i < 5; i++)
      {
        if (rcomm[i][0] == '>')
        {
          pipewdr = 1;
          outfile = rcomm[i + 1];
          rcomm[i] = 0;
          rcomm[i + 1] = 0;
          break;
        }
      }
      break;
    }

    if (comm[i][0] == '<' && comm[i + 2][0] == '>')
    {
      cmdtype = 5;
      inpfile = comm[i + 1];
      outfile = comm[i + 3];
      comm[i] = 0;
      comm[i + 1] = 0;
      comm[i + 2] = 0;
      comm[i + 3] = 0;
      reflag = 1;
    }

    if (comm[i][0] == '&' && comm[i + 1][0] == '&')
    {
      cmdtype = 8; // successive execution &&
      for (int j = 0; j < i; j++)
        lcomm[j] = comm[j];
      for (int j = i + 2, k = 0; j <= totcmd; j++, k++)
        rcomm[k] = comm[j];
      break;
    }

    if (comm[i][0] == '|' && comm[i + 1][0] == '|')
    {
      cmdtype = 9; // successive execution ||
      for (int j = 0; j < i; j++)
        lcomm[j] = comm[j];
      for (int j = i + 2, k = 0; j <= totcmd; j++, k++)
        rcomm[k] = comm[j];
      break;
    }

    if (comm[i][0] == '<' && reflag == 0 && spcount < 2)

    {
      // printf(2,"ctype 3");
      cmdtype = 3;
      inpfile = comm[i + 1];
      comm[i] = 0;
      comm[i + 1] = 0;
    }

    if (comm[i][0] == '>' && reflag == 0 && spcount < 2)
    {
      cmdtype = 4;
      outfile = comm[i + 1];
      comm[i] = 0;
      comm[i + 1] = 0;
    }

    if (comm[i][0] == ';')
    {
      cmdtype = 6;
      for (int j = 0; j < i; j++)
        lcomm[j] = comm[j];
      for (int j = i + 1, k = 0; j <= totcmd; j++, k++)
        rcomm[k] = comm[j];
      break;
    }

    if (strcmp(comm[0], "executeCommands") == 0)
    {
      cmdtype = 7;
    }
  }
}
int leftexec = 0, rightexec = 0;

void excmd()
{
  int pipefd[2];
  int fd;
  char filecmd[200];
  char *linecmd[10];
  linecmd[0] = malloc(sizeof(char) * 70);
  int status;
  int pfdo, pfdi;
  int line = 1;
  switch (cmdtype)
  {
  case 1: // normal execution
    exec(comm[0], comm);
    break;
  case 2: // pipe commands
          // printf(2,"\ngoing to pipe\n");
    if ((strcmp(lcomm[0], "ls") != 0 && strcmp(lcomm[0], "wc") != 0 && strcmp(lcomm[0], "cat") != 0 && strcmp(lcomm[0], "grep") != 0 &&
         strcmp(lcomm[0], "echo") != 0 && strcmp(lcomm[0], "ps") != 0 && strcmp(lcomm[0], "procinfo") != 0 && strcmp(lcomm[0], "executeCommands") != 0) &&
        (strcmp(rcomm[0], "ls") != 0 && strcmp(rcomm[0], "wc") != 0 && strcmp(rcomm[0], "cat") != 0 && strcmp(rcomm[0], "grep") != 0 &&
         strcmp(rcomm[0], "echo") != 0 && strcmp(rcomm[0], "ps") != 0 && strcmp(rcomm[0], "procinfo") != 0 && strcmp(rcomm[0], "executeCommands") != 0))
    {
      printf(2, "Illegal command or arguments\n");
      exit(0);
    }
    if (strcmp(rcomm[0], "ls") != 0 && strcmp(rcomm[0], "wc") != 0 && strcmp(rcomm[0], "cat") != 0 && strcmp(rcomm[0], "grep") != 0 &&
        strcmp(rcomm[0], "echo") != 0 && strcmp(rcomm[0], "ps") != 0 && strcmp(rcomm[0], "procinfo") != 0 && strcmp(rcomm[0], "executeCommands") != 0)
    {
      printf(2, "Illegal command or arguments\n");
      exit(0);
    }

    pipe(pipefd);
    if (fork() == 0)
    {
      if (strcmp(lcomm[0], "ls") != 0 && strcmp(lcomm[0], "wc") != 0 && strcmp(lcomm[0], "cat") != 0 && strcmp(lcomm[0], "grep") != 0 &&
          strcmp(lcomm[0], "echo") != 0 && strcmp(lcomm[0], "ps") != 0 && strcmp(lcomm[0], "procinfo") != 0 && strcmp(lcomm[0], "executeCommands") != 0)
      {
        printf(2, "Illegal command or arguments\n");
        exit(0);
      }
      close(1);
      dup(pipefd[1]);
      close(pipefd[0]);
      close(pipefd[1]);
      if (piperdr == 0)
        exec(lcomm[0], lcomm);
      else
      {
        close(0);
        pfdi = open(inpfile, O_RDONLY);
        if (pfdi < 0)
        {
          printf(2, "Illegal command or arguments\n");
          exit(0);
        }
        dup(pfdi);
        exec(lcomm[0], lcomm);
        close(pfdi);
      }
    }
    if (fork() == 0)
    {
      close(0);
      dup(pipefd[0]);
      close(pipefd[0]);
      close(pipefd[1]);
      if (pipewdr == 0)
        exec(rcomm[0], rcomm);
      else
      {
        close(1);
        pfdo = open(outfile, O_WRONLY | O_CREATE);
        dup(pfdo);
        exec(rcomm[0], rcomm);
        close(pfdo);
      }
    }
    close(pipefd[0]);
    close(pipefd[1]);
    wait(0);
    wait(0);
    break;
  case 3: // < redirection
          // for(int i=0;i<10;i++)
          //           printf(2,"%s\n",comm[i]);
    close(0);
    pfdi = open(inpfile, O_RDONLY);
    if (pfdi < 0)
    {
      printf(2, "Illegal command or arguments\n");
      exit(0);
    }
    dup(pfdi);
    exec(comm[0], comm);
    close(pfdi);
    break;
  case 4: // > redirection

    // for(int i=0;i<10;i++)
    //     printf(2,"%s\n",comm[i]);
    close(1);
    pfdo = open(outfile, O_WRONLY | O_CREATE);
    dup(pfdo);
    exec(comm[0], comm);
    close(pfdo);
    break;
  case 5: // <> redirection

    close(0);
    pfdi = open(inpfile, O_RDONLY);
    if (pfdi < 0)
    {
      printf(2, "Illegal command or arguments\n");
      exit(0);
    }
    dup(pfdi);
    close(1);
    pfdo = open(outfile, O_WRONLY | O_CREATE);
    dup(pfdo);

    exec(comm[0], comm);
    close(pfdi);
    close(pfdo);
    break;
  case 6: // ; parallel exection

    if (strcmp(lcomm[0], "ls") != 0 && strcmp(lcomm[0], "wc") != 0 && strcmp(lcomm[0], "cat") != 0 && strcmp(lcomm[0], "grep") != 0 &&
        strcmp(lcomm[0], "echo") != 0 && strcmp(lcomm[0], "ps") != 0 && strcmp(lcomm[0], "procinfo") != 0 && strcmp(lcomm[0], "executeCommands") != 0)
      printf(2, "Illegal command or arguments\n");
    else
    {

      if (fork() == 0)
      {
        //  int cid1=getpid();
        if (strcmp(lcomm[0], "executeCommands") == 0)
        {
          cmdtype = 7;
          leftexec = 1;
          excmd();
        }
        else
          exec(lcomm[0], lcomm);
      }
      // printf(2,"wait1 : %d",wait(0));
      // while (cid1 != wait(0))
      wait(0);
    }
    if (strcmp(rcomm[0], "ls") != 0 && strcmp(rcomm[0], "wc") != 0 && strcmp(rcomm[0], "cat") != 0 && strcmp(rcomm[0], "grep") != 0 &&
        strcmp(rcomm[0], "echo") != 0 && strcmp(rcomm[0], "ps") != 0 && strcmp(rcomm[0], "procinfo") != 0 && strcmp(rcomm[0], "executeCommands") != 0)
    {
      printf(2, "Illegal command or arguments\n");
      exit(0);
    }
    if (fork() == 0)
    {
      // int cid2=getpid();
      if (strcmp(rcomm[0], "executeCommands") == 0)
      {
        cmdtype = 7;
        rightexec = 1;
        excmd();
      }
      else
        exec(rcomm[0], rcomm);
    }
    // printf(2,"%d\n",cid2);
    // printf(2,"wait2 : %d",wait(0));
    wait(0);
    break;
  case 7: // executecommands from file
    // printf(2,"file exec start\n");
    if (leftexec == 1)
      fd = open(lcomm[1], O_RDONLY);
    else if (rightexec == 1)
      fd = open(rcomm[1], O_RDONLY);
    else
      fd = open(comm[1], O_RDONLY);
    // int EOF;
    read(fd, filecmd, 200);
    for (int i = 0; i < strlen(filecmd); i++)
    {
      if (filecmd[i] == '\n')
      {
        // newline[line]=i;
        line++;
      }
    }
    // printf(2,"No of lines: %d\n",line);
    int j = 0, k = 0;
    for (int i = 0; i < line; i++)
    {
      linecmd[i] = 0;
    }

    for (int i = 0; i < strlen(filecmd); i++)
    {
      if (filecmd[i] == '\r')
        continue;
      if (filecmd[i] != '\n')
      {
        // printf(2,"%c",filecmd[i]);
        linecmd[j][k] = filecmd[i];
        k++;
      }
      else
      {
        linecmd[j][k] = '\n';
        // linecmd[j+1]=linecmd[j];
        j++;
        k = 0;
        // i++;
        linecmd[j] = malloc(sizeof(char) * 70);
      }
    }
    linecmd[j][k] = '\n';

    // for(int i=0;i<line;i++)
    // {
    //   printf(2,"%s\n",linecmd[i]);
    // }

    for (int rt = 0; rt < line; rt++)
    {
      // printf(2, "%s %d", linecmd[rt], strlen(linecmd[rt]));

      cmdtype = 1;
      parseinp(linecmd[rt]);
      if (strcmp(comm[0], "exit") == 0)
      {
        exit(0);
      }
      if (strcmp(comm[0], "ls") != 0 && strcmp(comm[0], "wc") != 0 && strcmp(comm[0], "cat") != 0 && strcmp(comm[0], "grep") != 0 &&
          strcmp(comm[0], "echo") != 0 && strcmp(comm[0], "ps") != 0 && strcmp(comm[0], "procinfo") != 0 && strcmp(comm[0], "executeCommands") != 0)
        printf(2, "Illegal command or arguments\n");
      else
      {
        if (fork() == 0)
        {
          excmd();
          exit(0);
        }
        wait(0);
      }
    }
    break;

  case 8: // && consecutive execution
    if (strcmp(lcomm[0], "ls") != 0 && strcmp(lcomm[0], "wc") != 0 && strcmp(lcomm[0], "cat") != 0 && strcmp(lcomm[0], "grep") != 0 &&
        strcmp(lcomm[0], "echo") != 0 && strcmp(lcomm[0], "ps") != 0 && strcmp(lcomm[0], "procinfo") != 0 && strcmp(lcomm[0], "executeCommands") != 0)
      printf(2, "Illegal command or arguments\n");
    else
    {
      if (fork() == 0)
      {
        if (strcmp(lcomm[0], "executeCommands") == 0)
        {
          cmdtype = 7;
          leftexec = 1;
          excmd();
        }
        else
          exec(lcomm[0], lcomm);
      }
      wait(&status);
    }

    // printf(2,"type8 status: %d\n",status);
    if (status == 0)
    {

      if (strcmp(rcomm[0], "ls") != 0 && strcmp(rcomm[0], "wc") != 0 && strcmp(rcomm[0], "cat") != 0 && strcmp(rcomm[0], "grep") != 0 &&
          strcmp(rcomm[0], "echo") != 0 && strcmp(rcomm[0], "ps") != 0 && strcmp(rcomm[0], "procinfo") != 0 && strcmp(rcomm[0], "executeCommands") != 0)
      {
        printf(2, "Illegal command or arguments\n");
        exit(0);
      }
      else
      {
        if (fork() == 0)
        {
          if (strcmp(rcomm[0], "executeCommands") == 0)
          {
            cmdtype = 7;
            rightexec = 1;
            excmd();
          }
          else
            exec(rcomm[0], rcomm);
        }
        wait(0);
      }
    }

    break;

  case 9: // || successive execution
    if (strcmp(lcomm[0], "ls") != 0 && strcmp(lcomm[0], "wc") != 0 && strcmp(lcomm[0], "cat") != 0 && strcmp(lcomm[0], "grep") != 0 &&
        strcmp(lcomm[0], "echo") != 0 && strcmp(lcomm[0], "ps") != 0 && strcmp(lcomm[0], "procinfo") != 0 && strcmp(lcomm[0], "executeCommands") != 0)
      printf(2, "Illegal command or arguments\n");
    else
    {
      if (fork() == 0)
      {
        if (strcmp(lcomm[0], "executeCommands") == 0)
        {
          cmdtype = 7;
          leftexec = 1;

          excmd();
        }
        else
          exec(lcomm[0], lcomm);
      }
      wait(&status);
    }

    // printf(2,"type8 status: %d\n",status);
    if (status == -1)
    {
      if (strcmp(rcomm[0], "ls") != 0 && strcmp(rcomm[0], "wc") != 0 && strcmp(rcomm[0], "cat") != 0 && strcmp(rcomm[0], "grep") != 0 &&
          strcmp(rcomm[0], "echo") != 0 && strcmp(rcomm[0], "ps") != 0 && strcmp(rcomm[0], "procinfo") != 0 && strcmp(rcomm[0], "executeCommands") != 0)
      {
        printf(2, "Illegal command or arguments\n");
        exit(0);
      }
      else
      {

        if (fork() == 0)
        {
          if (strcmp(rcomm[0], "executeCommands") == 0)
          {
            cmdtype = 7;
            rightexec = 1;
            excmd();
          }
          else
            exec(rcomm[0], rcomm);
        }
        wait(0);
      }
    }

    break;
  }

  exit(0);
}

int main(void)
{
  char inp[200];

  while (1)
  {
    int filedesc;
    filedesc = open("console", O_RDWR);
    while (1)
    {
      if (filedesc > 2)
      {
        close(filedesc);
        break;
      }
    }

    printf(2, "Myshell> ");
    memset(inp, 0, sizeof(inp));
    gets(inp, sizeof(inp));
    cmdtype = 1;
    piperdr = 0;
    pipewdr = 0;
    parseinp(inp);

    // for (int i = 0; i < 10; i++)
    //   printf(2, "%s\n", comm[i]);
    // for (int i = 0; i < 5; i++)
    //   printf(2, "%s\n", lcomm[i]);
    // for (int i = 0; i < 5; i++)
    //   printf(2, "%s\n", rcomm[i]);
    // printf(2, "%d\n", strlen(comm[0]));

    if ((strcmp(comm[0], "exit") == 0))
    {
      wait(0);
      break;
    }

    if (strlen(rcomm[0]) > 0)
    {
      if (fork() == 0)
      {
        excmd();
        exit(0);
      }
      wait(0);
    }
    else
    {
      if (strcmp(comm[0], "ls") != 0 && strcmp(comm[0], "wc") != 0 && strcmp(comm[0], "cat") != 0 && strcmp(comm[0], "grep") != 0 &&
          strcmp(comm[0], "echo") != 0 && strcmp(comm[0], "ps") != 0 && strcmp(comm[0], "procinfo") != 0 && strcmp(comm[0], "executeCommands") != 0)
        printf(2, "Illegal command or arguments\n");
      else
      {
        if (fork() == 0)
        {
          excmd();
          exit(0);
        }
        wait(0);
      }
    }
  }
  exit(0);
}
