
#include<ulib.h>
// Dont try to add any other header files.
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
   //5---------------------------------
   // int pid;
   //  char buf1[20];
   //  int pipes[2];
   //  char *filename="test.txt";
    
   //  int create_fd = open(filename, O_CREAT|O_RDWR, O_READ|O_WRITE);
   //  char *P_buf = "Hello, I am file!";
   //  printf("P:%d\n", write(create_fd, P_buf, 17));
    
   //  if(pipe(pipes) == -1)
   //  {
   //      return 0;
   //  }

   //  int fd1 = dup(create_fd);
   //  pid = fork();
    
   //  if(pid == 0){
   //      char *buf = "Hello, I am Cpipe!";

   //      close(pipes[0]);
   //      printf("C:%d\n", write(pipes[1], buf, 18));


   //      printf("C:%d\n",lseek(fd1, 0, SEEK_SET));
   //      printf("C:%d\n", write(fd1, buf, 18));
   //      printf("C:%d\n",lseek(fd1, 0, SEEK_CUR));


   //      close(pipes[1]);
   //      printf("C:%d\n", get_stats());
   //      close(fd1);
   //      exit(0);
   //  }else{

   //      sleep(50);
   //      int fd_dup2 = dup2(pipes[0], 15);

   //      printf("P:%d\n", get_stats());
        
   //      close(pipes[1]);
   //      printf("P:%d\n", read(fd_dup2, buf1, 18));
   //      printf("P:%s\n",buf1);

   //      char buf2[36];
   //      printf("P:%d\n", write(create_fd, buf1, 18));

   //      lseek(fd1, 0, SEEK_SET);

   //      printf("P:%d\n", read(create_fd, buf2, 35));
   //      buf1[35] = 0;
   //      printf("P:%s\n", buf2);

   //      close(pipes[0]);
   //      close(fd1);
   //      close(fd_dup2);
   //      close(create_fd);
   //      printf("P:%d\n",get_stats());
   //      return 0;
   //  }
   //4---------------------------------

    // int pid;
    // char buf1[20];
    // char *file1="test.txt";
    // char *file2="test2.txt";

    // char *buf = "Hello, I am file!";
    // int fd_1 = open(file1, O_CREAT|O_RDWR, O_READ|O_WRITE);
    // printf("%d\n",get_stats());
    
    // int fd_2 = open(file1, O_READ); 
    // printf("%d\n",get_stats());
    
    // int fd_3 = open(file2, O_CREAT|O_RDWR, O_READ|O_WRITE);
    // printf("%d\n",get_stats()); 
    
    // int fd_4 = open(file2, O_CREAT|O_RDWR, O_READ|O_WRITE);
    // printf("%d\n",get_stats()); 



    // close(fd_1);
    // printf("%d\n",get_stats()); 
    // close(fd_2);
    // printf("%d\n",get_stats()); 
    // close(fd_3);
    // //Below printf will be printing 3 because of Standard file descriptor(STDIN, STDOUT, STDERROR)
    // printf("%d\n",get_stats());
    // return 0;

   // int pid;
   //  char buf1[20];
   //  char *file1="test.txt";
   //  char *file2="test2.txt";

   //  char *buf = "Hello, I am file!";
   //  int fd_1 = open(file1, O_CREAT|O_RDWR, O_READ|O_WRITE);
   //  printf("%d\n",get_stats());
    
   //  int fd_2 = open(file1, O_RDONLY); 
   //  printf("%d\n",get_stats());
    
   //  int fd_3 = open(file2, O_CREAT|O_RDWR, O_READ|O_WRITE);
   //  printf("%d\n",get_stats()); 
    
   //  // This will not create any file descriptor as the file with the name "test2.txt" was created already.
   //  int fd_4 = open(file2, O_CREAT|O_RDWR, O_READ|O_WRITE);
   //  printf("%d\n",get_stats()); 

   //  //int fd5 = dup(fd_4);
   //  //printf("D:%d\n",get_stats());
   //  pid = fork();
   //  if(pid==0)
   //  {
   //    close(fd_1);
   //    printf("C:%d\n",get_stats()); 
   //    close(fd_2);
   //    printf("C:%d\n",get_stats()); 
   //    close(fd_3);
   //    printf("C:%d\n",get_stats());
   //    close(fd_4);
   //    exit(0);
   //  }
   //  else
   //  {
   //    sleep(50);
   //    close(fd_1);
   //    printf("P:%d\n",get_stats()); 
   //    close(fd_2);
   //    printf("P:%d\n",get_stats()); 
   //    close(fd_3);
   //    printf("P:%d\n",get_stats());
   //  }
   //  close(fd_4);
   //  //close(fd_5);
   //  //Below printf will be printing 3 because of Standard file descriptor(STDIN, STDOUT, STDERROR)
   //  printf("O:%d\n",get_stats());
   //  return 0;
   //3---------------------------------
   int pid;
  char buf1[20];
  char buf2[20];
  int pipes[2];

  if(pipe(pipes) == -1)
  {
    return 0;
  }

  pid = fork();
  
  if(pid == 0){
     char *buf = "Hello, I am file!";
     char *buf0 = "22222Hello, I am file!";
     close(pipes[0]);
     printf("C:%d\n", write(pipes[1], buf, 17));
     printf("C:%d\n", write(pipes[1], buf0, 7));

     // Below printf will be printing -1, Because read fd is closed already
     printf("C:%d\n", read(pipes[0], buf1, 5));
     exit(0);
  }else{
     sleep(50);
     close(pipes[1]);
     
     printf("P:%d\n", read(pipes[0], buf1, 16));
      printf("P:%d\n", read(pipes[0], buf2, 10));
    // Below printf will be printing -1, Because write fd is closed already
     printf("P:%d\n", write(pipes[1], buf1, 10));
     //buf1[20] = 0;
     printf("P:%s\n", buf1);
     printf("P:%s\n", buf2);
     close(pipes[0]);
  }
   //2---------------------------------
   // int pid;
   //  char buf1[20];
   //  char *filename="test.txt";
   //  int create_fd = open(filename, O_CREAT|O_RDWR, O_READ|O_WRITE);
   //  char *buf = "Hello, I am file!";
   //  write(create_fd, buf, 17);

   //  int dup_fd = dup(create_fd);
   //  printf("%d\n", dup_fd);

   //  int dup_fd2 = dup2(create_fd, 8);

   //  printf("%d\n", dup_fd2);

   //  printf("%d\n", lseek(create_fd, 0, SEEK_CUR));
   //  return 0;
   //1---------------------------------
   // int pid;
   //  char buf1[20];
   //  char *filename="test.txt";
   //  int create_fd = open(filename, O_CREAT|O_READ, O_READ|O_WRITE);
   //  char *buf = "Hello, I am file!";
   //  printf("%d\n", write(create_fd, buf, 17));

   //  int read_fd = open(filename, O_RDONLY);
   //  printf("%d\n", read(read_fd, buf1, 27));
   //  buf1[17] = '\0';
   //  printf("%s\n",buf1);
   //  close(read_fd);
   //  close(create_fd);
   //  return 0;
   // 0----------------------------------
   // int a = 10, b = 20;
   // printf("%d\n", a+b);
   
   // int pid = getpid();
   // printf("Pid %d\n", pid);
   
   // pid = fork();
   // if(pid == 0)
   // {
   //    printf(" OS 1234C330 Child1\n");
   //    exit(0);
   // }
   // return 0;
}