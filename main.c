/*
 *
 *    Isletim Sistemleri Proje  : 09/12/2018
 *		B151210066 Hasan Aydoğmuş
 *			
 */
#include <stdio.h>

#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>			
#include <sys/stat.h>		
#include <errno.h>
#include "main.h"


int main( int argc, char *argv[], char *env[] )
{
	int status;
	char* komut;
	char* temp;
	pid_t pid;
	char *array[MAX_SIZE];
	int size,flag=0,retVal=0,i,errno;
	size_t bufsize=1000;
	signal(SIGINT, signal_callback_handler);

	temp = (char *)malloc(1000 * sizeof(char));
	do{
		komut = (char *)malloc(1000 * sizeof(char));
		komut=prompt(komut, &bufsize);
	    strToAr(komut, array, &size);
	 	flag=0;
		for (i = 0; i < size; ++i){
			if (array[i][0]=='&'){
				flag = 1;
			}		
		}
	    if((pid = fork()) < 0 )
	    {
		  perror("fork failure");
		  exit(1);
	    }
		/*
		 * child process
		 */
	    if(pid == 0)
	    {  
	    	signal(SIGINT, signal_callback_handler);
		 	
		 	retVal=komutlar(array, size, flag);

		  	_exit(1);
	    }
		/*	
		 * parent process
		 */
	    else
	    {
          signal(SIGINT, signal_callback_handler);signal(SIGINT, signal_callback_handler);
          
		   if ((strcmp(komut, "quit") == 0 ) || (strcmp(komut, "quit\n") == 0 ) ){
		   		fprintf(stderr, "Child proseslerin sonlanması bekleniyor...\n");
				while (pid = waitpid(-1, NULL, 0)) {
					if (errno == ECHILD) {
						break;
	   				}
				}
		   		fprintf(stderr, "Program sonlandırıldı.\n");
		   		free(komut);	
		   		exit(0);				
			}
			
            if (flag ==0)
			{	/* normal çalışma */				
				wait(NULL);	
			}
			else if (flag ==1)
			{	/* arkaplanda çalışma */
				//[pid] retval: <exitcode>
				fprintf(stderr, "%d %s: %d\n",getpid(),"retval",retVal);
			}
	    }
	    strcpy(temp, komut);
	    free(komut);
    }while(!(strcmp(temp, "quit") == 0 ) || (strcmp(temp, "quit\n") == 0 ));
   return 0;
}

char * prompt(char * komut, size_t *bufsize){
  fflush(stdout);
  printf(">");
  fflush(stdout);
  getline(&komut,bufsize,stdin);
  return komut;
}

void signal_callback_handler(int signum)
{
   printf("Caught signal %d\n",signum);
   // Cleanup and close up stuff here
   // Terminate program
   exit(signum);
}

void strToAr(char* string, char *array[MAX_SIZE],int *size){

  char *str = string;
  char * pch;
  int si=0;
  int i=0,j=0;
  for(i=0; string[i]!='\0';++i){
  	if(string[i] == '\n')
  		string[i]='\0';
  }
  pch = strtok(str," ");
  array[si++]=pch;

  while (pch != NULL)
  {
    pch = strtok (NULL, " ");
    array[si++]=pch;
  }
  --si;
  *size = si;
  /*
  for(i=0; i<*size;++i)
  	fprintf(stderr,"[%s]",array[i]);*/
}

//flg & varsa 1 
//size arrayin size
//array icinde girilen komutlarin tutuldugu string arrayi 
//execlerle yapilacak
//3-4-5 buranin icinde olacak 
int komutlar(char* array[MAX_SIZE],int size,int flg){
	int fileOut,i;
	
	//sleep(6); // Quit komutu geldiğinde parent prosesin child prosesleri beklediğini görmek icin aktif edilecek
	if(flg==1)
		size = size -1;
	//PART 3-Tekil Komutlar
	//ls komutu
    if(strcmp(array[0] ,"ls") == 0)
    {
    //char *argv[] = { "ls", "-l", 0 };
    //execvp(argv[0], argv);
        execl("/bin/ls", "ls", "-l", (char *)0);
    }
    //echo komutu
    else if(strcmp(array[0] ,"echo") == 0)
    {
        i = 1;
        printf("Ekrana yazdiginiz metin: ");
        while(i != size){
            printf("%s ", array[i]);
            ++i;
        }
        printf("\n");
        //system("echo 'words' > file");
    }
    //pwd komutu
    else if(strcmp(array[0] ,"pwd") == 0)
    {
        char cwd[1024];
        chdir("/path/to/change/directory/to");
        getcwd(cwd, sizeof(cwd));
        printf("Current working dir: %s\n", cwd);
    }
    
    //PART 4 & 5
	//cat komutu
	else if(strcmp(array[0],"cat")==0){
		switch(size){
			case 2:
				execl("/bin/cat",array[0],array[1],(char *)NULL);
				break;
			case 3:
				execl("/bin/cat",array[0],array[2],(char *)NULL);
				break;
			
			case 4:
				if(strcmp(array[2] ,">") == 0)
				{
					/***********************************************/
					fileOut = open(array[3], O_APPEND | O_WRONLY);	
					//Now we redirect standard output to the file using dup2
					if(dup2(fileOut,1) < 0)    
					  return 1;
					  
					execl("/bin/cat",array[0],array[1],(char *)NULL);
					perror("execl() failure!\n\n");
					close(fileOut);
					/***********************************************/
				}
				else if(strcmp(array[2] ,"<") == 0)
				{
					/***********************************************/
					fileOut = open(array[1], O_APPEND | O_WRONLY);	
					//Now we redirect standard output to the file using dup2
					if(dup2(fileOut,1) < 0)    
					  return 1;
					  
					execl("/bin/cat",array[0],array[3],(char *)NULL);
					perror("execl() failure!\n\n");
					close(fileOut);
					/***********************************************/
				}	
				break;
				
		}
	}
	return 0;
}

