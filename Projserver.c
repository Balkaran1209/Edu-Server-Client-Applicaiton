#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <semaphore.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>


#define MSG_VAL_LEN  16
#define CLIENT_Q_NAME_LEN 16
#define MSG_TYPE_LEN 16

typedef struct
{
	char client_q[CLIENT_Q_NAME_LEN];
	char msg_val[MSG_VAL_LEN];
}client_msg_t;

typedef struct
{
	char msg_type[MSG_TYPE_LEN];
	char msg_val[MSG_VAL_LEN];
} server_msg_t;

typedef struct
{
	int is_deleted;
	int course_id;
	int teacher_id;

}Courses;

typedef struct
{
	int is_deleted;
	int teacher_id;
}Teachers;


sem_t bin_sem;
static client_msg_t client_msg;
int num_of_courses = 0;
int num_of_cor = 0;
int num_of_teachers = 0;
int num_of_tch = 0;

Courses course_records[700];
Teachers teacher_records[700];



#define SERVER_QUEUE_NAME   "/server_msgq"
#define QUEUE_PERMISSIONS 0660
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE sizeof(client_msg_t)
#define MSG_BUFFER_SIZE (MAX_MSG_SIZE * MAX_MESSAGES) 



void* thread_function(void* Thread) 
{
	while(1) 
    {
		sem_wait(&bin_sem);
		for(int i=0;i<num_of_courses;i++)
		{
			if (course_records[i].is_deleted == 0)
			{ 
				printf("COURSE ID: %d, TEACHER ID: %d\n", course_records[i].course_id, course_records[i].teacher_id);
			}
		}
		printf("\n");
		for(int j=0;j<num_of_teachers;j++)
		{
			if (teacher_records[j].is_deleted == 0)
			{ 
				printf("TEACHER ID: %d\n", teacher_records[j].teacher_id);
			}
		}
		sem_post(&bin_sem);
		sleep(5);
	}
}
 


int duplicate_courses(int course_id) 
{
	for (int i=0;i<num_of_courses;i++) 
    {
		if(course_records[i].course_id == course_id && course_records[i].is_deleted == 0) 
        {
			return 1;
		}
	}
	return 0;
}



int duplicate_teachers(int teacher_id) 
{
	for (int i=0;i<num_of_teachers;i++) 
    {
		if(teacher_records[i].teacher_id == teacher_id && teacher_records[i].is_deleted == 0) 
        {
			return 1;
		}
	}	
	return 0;
}



int oper(char *array[], int count) 
{
	char *ADD = "add";
	char *DEL = "del";
	char *COR = "course";
	char *TCH = "teacher";
	if (*array[0]==*ADD) 
    {
		if (*array[1]==*COR) 
        {
			int pos = 2;
			while(array[pos]) 
            {
				int value = strtol(array[pos], NULL, 10);
				int dup = duplicate_courses(value);
				if (dup == 1) 
                {
					printf("Entered course ID already exist.");
					printf("\n");
				} 
                else 
                {
					int ID = strtol(array[pos], NULL, 10);
					int curr = rand() % num_of_tch;
					int tempDup = duplicate_teachers(curr);
					Courses newCourse = {0, value, curr};
					if(tempDup == 1) 
                    {
						if(num_of_courses<700) 
                        {
							course_records[num_of_courses++] = newCourse;
                            num_of_cor++;
						} 
                        else 
                        {
							for (int i = 0; i < num_of_courses ; i++) 
                            {
								if (course_records[i].is_deleted == 1) 
                                {
									course_records[i] = newCourse;
								}
							}
						}
					}
				}
				pos++;
			}
		} 
        else if(*array[1]==*TCH) 
        {
			int pos = 2;
			while(array[pos]) 
            {
				int value = strtol(array[pos], NULL, 10);
				int dup = duplicate_teachers(value);

				if (dup == 1) 
                {
					printf("Entered teacher ID already exist.");
					printf("\n");
				} 
                else 
                {
					Teachers newTeacher = {0, value};		
					if (num_of_teachers < 700) 
                    {
						teacher_records[num_of_teachers++] = newTeacher;
                        num_of_tch++;
					}                  
                    else 
                    {
						for (int i = 0; i < 700; i++) 
                        {
							if (teacher_records[i].is_deleted == 1) 
                            {
								teacher_records[i] = newTeacher;
							}
						}
					}
				}
				pos++;
			}
		} 
        else 
        {
			printf("Invalid input. Please enter a valid input.\n");
		}

	} 
    
    else if (*array[0] == *DEL) 
    {
		if (*array[1] == *COR) 
        {
			int pos = 2;		
			while (array[pos]) 
            {
				int n;
				int value = strtol(array[pos], NULL, 10);
				if (num_of_courses < 700) 
                {
					n = num_of_courses;
				} 
                else
                {
					n = 700;
				}
				for (int i = 0;i<n;i++) 
                {
					if (course_records[i].course_id == value && course_records[i].is_deleted == 0) 
                    {
						course_records[i].is_deleted = 1;
                        num_of_cor--;
					}
				}
				pos++;
			}
		} 

        else if(*array[1] == *TCH) 
        {
        	int pos = 2;	
			while (array[pos]) 
            {
				int n;
				int value = strtol(array[pos], NULL, 10);
				if (num_of_teachers < 700) 
                {
					n = num_of_teachers;
				} 
                else
                {
					n = 700;
				}
				int teacher_deleted = -1;
				for(int i = 0; i < n; i++) 
                {
					if (teacher_records[i].teacher_id == value && teacher_records[i].is_deleted == 0) 
                    {
						teacher_records[i].is_deleted = 1;
						teacher_deleted = value;
                        
						num_of_tch--;
						
						break;
					}
				}
				int random_index = rand() % num_of_tch;
				while(teacher_records[random_index].is_deleted == 1)
				{
					random_index = rand() % num_of_tch;
				}
				int start=0;
				for(int i=0; i<num_of_courses; i++) 
				{
					if(course_records[i].teacher_id == teacher_deleted) 
					{
						course_records[i].teacher_id = random_index;
					}
				}
				pos++;
			}

		} 
        else 
        {
			printf("Invalid input. Please enter a valid input.\n");
		}
	} 
	return 0;
}

int main (int argc, char **argv)
{
    mqd_t qd_srv;
    mqd_t qd_client;   
    int num = 1; 
    int res_sem; 
    int sem;
    pthread_t thread; 
    res_sem = sem_init(&bin_sem, 0, 1);
    if(res_sem != 0) 
    {
    	printf("Semaphore creation has failed: %d \n", res_sem);
    	exit(1);
    }

    if((sem = pthread_create(&thread, NULL, &thread_function, "Report summary thread"))) 
    {
    	printf("Thread creation failed: %d \n", sem);
    	exit(1);
    }
    printf ("Server MsgQ: Welcome!!!\n");
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;
    if((qd_srv = mq_open(SERVER_QUEUE_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr))==-1) 
    {
        perror ("Server MsgQ: mq_open (qd_srv)");
        exit (1);
    }
    client_msg_t in_msg;
	char* val_client;
	char* quit = "exit";
    while (1) 
    {
        if (mq_receive(qd_srv,(char *) &in_msg, MAX_MSG_SIZE, NULL) == -1) 
        {
            perror ("Server msgq: mq_receive");
            exit (1);
        }
	    val_client = in_msg.msg_val;
        printf ("%d: Server MsgQ: message received.\n", num);
        printf("Client msg q name = %s\n", in_msg.client_q);
        printf("Client msg val = %s\n", val_client);
        if (*val_client == *quit) 
        {
        	printf("Exiting the server. Thank You!!\n");
        	break;
        }
        char *tokenizedString[100];
	    int count = 0;
    	tokenizedString[count++] = strtok(val_client, " ");
        while(tokenizedString[count - 1] != NULL) 
        {
            tokenizedString[count++] = strtok(NULL, " "); 
        }
        int operation = oper(tokenizedString, count);
        char* out;
        if (operation == 0) 
        {
            out = "Successful!\n";
        } 
        else 
        {
            out = "Failed to do the operation!\n";
        }	
        printf("Number of Courses added: %d \n", num_of_cor);
        printf("Number of Teachers added: %d \n", num_of_tch);
        server_msg_t out_msg; 
        strcpy(out_msg.msg_type, "Server msg");  
        sprintf (out_msg.msg_val, "%s", out);    	             		       

            // Open the client queue using the client queue name received

        if ((qd_client = mq_open(in_msg.client_q, O_WRONLY)) == 1) 
        {
            perror ("Server MsgQ: Not able to open the client queue");
            continue;
        }     	

        

            // Send back the value received + 10 to the client's queue           

            // int mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned int msg_prio);

        if (mq_send(qd_client, (char *) &out_msg, sizeof(out_msg), 0) == -1) 
        {
            perror ("Server MsgQ: Not able to send message to the client queue");
            continue;
        }  

            //sem_post(&bin_sem);    

    } // end of while(1) 

}  // end of main()




