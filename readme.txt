RUN

Server
gcc -o server Projserver.c -lrt -pthread
./server

Client
gcc -o client Projclient.c -lrt -pthread
./client

***************************************************

Adding teacher
add tch teacher_id

Adding course
add cor course_id

Deleting teacher
del tch teacher_id

Deleting course
del tch course_id
