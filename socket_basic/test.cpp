#include <iostream>
#include <memory>
#include <string>
#include <unistd.h>

#include "test_server/socket_basic/proto/helloworld.pb.h"

using helloworld::HelloRequest;
using helloworld::HelloReply;

int main(){
    HelloRequest request;
    request.set_name("Knightwu");
    std::cout << request.name() << std::endl;


    char a1[100] = "apple";
    char a2[100] = "blue";
    char a3[100] = "red";
    int a1_length = strlen(a1);
    int a2_length = strlen(a2);
    int a3_length = strlen(a3);
    printf("a1:%s, length:%d.\n", a1, a1_length);
    printf("a2:%s, length:%d.\n", a2, a2_length);
    printf("a3:%s, length:%d.\n", a3, a3_length);
    printf("int size:%d.\n", sizeof(int));

    char *ptr = (char *)malloc(sizeof(char) *10000);
    char *tmp_ptr = ptr;
    memcpy(tmp_ptr, &a1_length, sizeof(int));
    tmp_ptr += sizeof(int);
    memcpy(tmp_ptr, a1, a1_length);
    tmp_ptr += a1_length;
    memcpy(tmp_ptr, &a2_length, sizeof(int));
    tmp_ptr += sizeof(int);
    memcpy(tmp_ptr, a2, a2_length);
    tmp_ptr += a2_length;
    memcpy(tmp_ptr, &a3_length, sizeof(int));
    tmp_ptr += sizeof(int);
    memcpy(tmp_ptr, a3, a3_length);

    char b1[100] = {0};
    char b2[100] = {0};
    char b3[100] = {0};
    int size = 0;
    tmp_ptr = ptr;
    memcpy(&size, tmp_ptr, sizeof(int));
    printf("b1 size:%d.\n", size);
    tmp_ptr += sizeof(int);
    memcpy(b1, tmp_ptr, size);
    tmp_ptr += size;
    size = 0;
    memcpy(&size, tmp_ptr, sizeof(int));
    tmp_ptr += sizeof(int);
    memcpy(b2, tmp_ptr, size);
    tmp_ptr += size;
    size = 0;
    memcpy(&size, tmp_ptr, sizeof(int));
    tmp_ptr += sizeof(int);
    memcpy(b3, tmp_ptr, size);
    tmp_ptr += size;

    printf("b1:%s, length:%d.\n", b1, strlen(b1));
    printf("b2:%s, length:%d.\n", b2, strlen(b2));
    printf("b3:%s, length:%d.\n", b3, strlen(b3));

    free(ptr);

    int * xx = (int *)malloc(sizeof(int));
    bzero(xx,sizeof(int));
    *xx = *xx +100;
    printf("xx:%d.\n", *xx);
    free(xx);
    xx = NULL;

    int row = 5;
    int col = 100;
    char ** data = (char **)malloc(sizeof(char *) * row);
    for (int i=0; i< row; i++) {
        data[i] = (char *)malloc(sizeof(char) * col);
        bzero(data[i], sizeof(char)*col);
    }
    strcpy(data[0], "This is xxxx.");
    std::cout << data[0] << std::endl;

    for (int i=0; i< row; i++) {
        free(data[i]);
    }
    free(data);

    return 0;
}
