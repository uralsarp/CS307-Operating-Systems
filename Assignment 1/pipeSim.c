#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
 
 printf("I’m SHELL process, with PID: (pid:%d) - Main command is: man ping | grep -A 1 -e v > output.txt\n", (int) getpid());
 
 int fd[2]; //file descriptor
 
 if(pipe(fd)==-1){ //pipe failed
  
  printf("Pipe failed");
  exit(1);
 
 }
 
 int child1 = fork();
 
 if (child1 < 0) { //fork failed
  
  printf("fork failed\n");
  exit(1);
 
 } 
 else if (child1 == 0) { //first child
  
  printf("I’m MAN process, with PID: (pid:%d) - My command is: man ping\n", (int) getpid());
  
  dup2(fd[1],STDOUT_FILENO); //give this processes' output to the pipe

  close(fd[0]);
  close(fd[1]);

  char *args1[3];
  args1[0] = strdup("man"); 
  args1[1] = strdup("ping"); 
  args1[2] = NULL; // mark end of array
  execvp(args1[0], args1);
  
 }
 
 else { //parent process
      
 	waitpid(child1,NULL,0); //child1 has to finish it's process for child2 to be created

 	int child2 = fork();

 	if (child2 < 0){ //fork failed
  	 
	 printf("fork failed\n");
  	 exit(1);
 	
	}

 	else if(child2 == 0) { //second child

  	 printf("I’m GREP process, with PID: (pid:%d) - My command is: grep -A 1 -e -v > output.txt\n", (int) getpid());
  
  	 dup2(fd[0],STDIN_FILENO); //get the output of the other process' from the pipe
  
 	 close(fd[0]);
  	 close(fd[1]);

  	 int output = open("output.txt",O_CREAT|O_WRONLY|O_TRUNC,S_IRWXU);
  	 dup2(output, STDOUT_FILENO);
  	 close(output); 
  
  	 char *args2[]={"grep","-A","1","-e","-v",NULL};
  	 execvp(args2[0],args2);
 	
	}

	else{ //parent process

 	 close(fd[0]);
 	 close(fd[1]); 
 	 waitpid(child1,NULL,0);
	 waitpid(child2,NULL,0);

 	 printf("I’m SHELL process, with PID: (pid:%d) - execution is completed, you can find the results in output.txt\n", (int) getpid());
        
	}
 }
 return 0;
}


