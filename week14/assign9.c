#include "stdafx.h"


#define MSGSIZE 6

char *msg1 = "hello";
char *msg2 = "bye!!";

void parent(int [][]);
int child(int []);

main()
{
int pip[6][2]; // 6 children processes are connected to the parent process via 6 pipes
int i;

for(i = 0; i < 6; i++) //"Instead of 3 children processes, create 6 children processes."
{
if(pipe(pip[i]) == -1)
fatal("pipe call");

switch(fork())
{
case -1:
fatal("fork call");
case 0:
child(pip[i]);
}
}

parent (pip);
exit(0);
}

void parent(int p[6][2])
{
char buf[MSGSIZE], ch;
fd_set set, master;
int i ;

for(i = 0; i < 6; i++)
close(p[i][1]);

FD_ZERO(&master);
FD_SET(0, &master);

for(i = 0; i < 6; i++)
FD_SET(p[i][0], &master);

while(set = master, select(p[2][0]+1, &set, NULL, NULL, NULL) > 0)
{
if(FD_ISSET(0, &set))
{
printf("From standard input...");
read(0, &ch, 1);
printf("%c\n", ch);
}

for(i = 0; i < 6; i++)
{
if(FD_ISSET(p[i][0], buf, &set))
{
if(read(p[i][0], buf, MSGSIZE)>0)
{
printf("Message from child%d\n", i);
printf("MSG=%s\n", buf);
}
}
}

if(waitpid(-1, NULL, WNOHANG) == -1)
return;
}
}

int child(int p[2])
{
int count;

close(p[0]);

for(count = 0; count < 2; count++)
{
write(p[1], msg1, MSGSIZE); //Child keeps writing to the pipe with 1 second apart between consecutive writes
sleep(1);
}

write(p[1], msg2, MSGSIZE);
exit(0);
}
