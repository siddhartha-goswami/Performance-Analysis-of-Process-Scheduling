/*___________Group42______________
Anshuman Bhagawati – 2018A3PS0610H
Iype Eldho – 2018AAPS0339H
Lakshay Tomar – 2018B1AA0578H
Kshitij Nayyar – 2019A8PS0420H
J Vigneshwara Reddy – 2019A3PS0447H
Prince Jain – 2018A3TS0906H
Siddhartha Goswami – 2018A3PS0523H  
Siddharth Raj – 2018AAPS0398H*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

static int *glob_var, *complete1, *complete2, *complete3;

void* addn1(void* param); //Function definition for thread
void* mon1(void* param);
void* mon2(void* param);
void* readn2(void* param);
void* mon3(void* param);
void* readaddn3(void* param);


int t1 = 0, t2 = 0, t3 = 0;

int main(){
    clock_t start, end;
    double cpu_time_used;
    start = clock();

    pid_t C1, C2, C3;
    int pipe1[2], pipe2[2], pipe3[2], pipe4[2], pipe5[2], pipe6[2];
    pipe(pipe1);
    pipe(pipe2);
    pipe(pipe3);
    pipe(pipe4);
    pipe(pipe5);
    pipe(pipe6);
    
    glob_var = mmap(NULL, sizeof *glob_var, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    complete1 = mmap(NULL, sizeof *complete1, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    complete2 = mmap(NULL, sizeof *complete2, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    complete3 = mmap(NULL, sizeof *complete3, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    *glob_var = 0;
    *complete1 = 0;
    *complete2 = 0;
    *complete3 = 0;

    C1 = fork();
    if(C1 == 0)  //Child 1
    {
    	while(*glob_var != 1);
    	int t,arr, t5;
    	pthread_t m1;
    	pthread_create(&m1, NULL, mon1, (void*)t);
        
        pthread_t thread;
        pthread_create(&thread, NULL, addn1, (void*)arr);
        int su;
        pthread_join(thread, (void*)&su);
        pthread_join(m1, NULL);
        
        close(pipe1[0]);
        write(pipe1[1], &su, sizeof(su));
        close(pipe1[1]);
    }
    else//Parent
    {
    	C2 = fork();
    	if(C2 == 0) //Child 2
    	{
        while(*glob_var != 2);
        
	int t,arr;
    	pthread_t m2;
    	pthread_create(&m2, NULL, mon2, (void*)t);
        
        pthread_t thread;
        pthread_create(&thread, NULL, readn2, (void*)arr);
        int* su;
        pthread_join(thread, (void*)&su);
        pthread_join(m2, NULL);
        char msg[] = "Done Printing";
        printf("\nchild2 %s\n", msg);
        close(pipe2[0]);
        write(pipe2[1], msg, (strlen(msg))+1);
        close(pipe2[1]);
        	
    	}
    	else//Parent Continues Here
    	{
    		C3 = fork();
    		if (C3 == 0) 
    		{
    			while(*glob_var != 3);
			int t,arr, t5;
    			pthread_t m3;
    			pthread_create(&m3, NULL, mon3, (void*)t);
        
        			pthread_t thread;
        			pthread_create(&thread, NULL, readaddn3, (void*)arr);
       				int su;
        			pthread_join(thread, (void*)&su);
        			pthread_join(m3, NULL);
        			
        			close(pipe3[0]);
        			write(pipe3[1], &su, sizeof(su));
        			close(pipe3[1]);
        			
    		}
    		else //Parent continues here
    		{
    			printf("\nParent process Start\n");
        		
        		int input;
        		printf("\nEnter 1 for FCFS, 2 for round robin : ");
        		scanf("%d", &input);
        		int c1res, c3res;
        		
        		//FCFS
        		if(input == 1){
        			*glob_var = 1;
        			while(!*complete1);
        			printf("\nChecking\n");
        			*glob_var = 2;
        			while(!*complete2);
        			//str = "3";
        			*glob_var = 3;
        			while(!*complete3);
        		}
        		
        		//Round Robin
        		if(input == 2){
        			int timequant, num = 0;
        			printf("\nEnter Time Quantum - ");
        			scanf("%d", &timequant);
        			
        			while(!*complete1 || !*complete2 || !*complete3){
        				num++;
        				if(num % 3 == 1){
        					if(*complete1 != 1){
        						printf("\nProcess1 Running\n");
        						*glob_var = 1;
        						
        						sleep(timequant);
        					}
        					else
        						continue;
        				}
        				else if(num % 3 == 2){
        					if(*complete2 != 1){
        						printf("\nProcess2 Running\n");
        						*glob_var = 2;
        						
        						sleep(timequant);
        					}
        					else
        						continue;
        				}
        				else if(num % 3 == 0){
        					if(*complete3 != 1){
        						printf("\nProcess3 Running\n");
        						*glob_var = 3;
        						
        						sleep(timequant);
        					}
        					else
        						continue;
        				}
        			}
        		}
        		close(pipe1[1]);
        		read(pipe1[0], &c1res, sizeof(c1res));
        		close(pipe1[0]);
        		printf("\nInside parent. received from C1 - %d\n", c1res);
        		
        		char c2res[100];
        		close(pipe2[1]);
        		read(pipe2[0], c2res, sizeof(c2res));
        		close(pipe2[0]);
        		printf("\nInside parent. received from C2 - %s\n", c2res); 
        		
        		close(pipe3[1]);
        		read(pipe3[0], &c3res, sizeof(c3res));
        		close(pipe3[0]);
        		printf("\nInside parent. received from C3 - %d\n", c3res);

    			printf("\nThank you for using our program\n");
    		}
	}
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Total time of program: %lf\n", cpu_time_used); 
    return 0;
}

void* mon1(void* param){
        while(1){
        	if(*glob_var == 1){
        		t1 = 1;
        	}
        	else{
        		t1 = 0;
        	}
        	if(*complete1 == 1)
        		break;
        }
        printf("\nProcess1 completed\n");
        pthread_exit(0);
}

void* addn1(void* param){
	int n1 = 0, sum1 = 0, num = 0;
	while(t1 != 1);
        printf("\nProcess C1: Enter the amount of numbers to add (between 25 and 1 million): ");
        scanf("%d", &n1);
        for(int i = 0; i < n1; i++){
		while(t1 != 1);

            printf("Enter number: ");
            scanf("%d", &num);
            sum1 = sum1 + num;
	}
	*complete1 = 1;
	pthread_exit((void*)sum1);
}

void* mon2(void* param){
  	while(1){
        	if(*glob_var == 2){
        		t2 = 1;
        	}
        	else{
        		t2 = 0;
        	}
        	if(*complete2 == 1)
        		break;
        }
        pthread_exit(0);
}

void* readn2(void* param){
//C2 Program
        int n2=0;
        FILE *myFile;
        myFile = fopen("numbers.txt", "r");
        if (myFile == NULL){
            printf("Error Reading File\n");
            exit (0);
        }
        printf("\nProcess C2: Enter the amount of numbers to print (between 25 and 1 million): ");
        scanf("%d", &n2);
        int numberArray[n2];
        int i;

        for (i = 0; i < n2; i++){
	while(t2 != 1){
        	}

            fscanf(myFile, "%d,", &numberArray[i] );
            printf("%d\n", numberArray[i]);
        }
        
        fclose(myFile);
	*complete2=1;
	pthread_exit(0);
}

void* mon3(void* param){
	while(1){
        	if(*glob_var == 3){
        		t3 = 1;
        	}
        	else{
        		t3 = 0;
        	}
        	if(*complete3 == 1)
        		break;
        }
        pthread_exit(0);
}

void* readaddn3(void* param){
int sum3 = 0, n3=0;
        FILE *myFile;
        myFile = fopen("numbers.txt", "r");
        if (myFile == NULL){
            printf("Error Reading File\n");
            exit (0);
        }
        printf("\nProcess C3: Enter the amount of numbers to add (between 25 and 1 million): ");
        scanf("%d", &n3);
        int numberArray[n3];
        int i;
        for (i = 0; i < n3; i++){
		while(t3 != 1);
		fscanf(myFile, "%d,", &numberArray[i] );
		sum3 = sum3 + (numberArray[i]);
        }
        fclose(myFile);
        *complete3 = 1;
	pthread_exit((void*)sum3);
}
