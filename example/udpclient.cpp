#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

float rand_float[10] = {17.5, 18, 19, 18.5, 16.2, 13.4, 16.5, 20, 13.3, 19.3};

int main() {
  int sockfd;
  struct sockaddr_in servaddr;
  
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(19999);
  inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
  
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);

  while (1) {
    time_t t = time(NULL);
    int rand_num = (int)(rand()%10);
    char data[1024] = {0};
    sprintf(data, "{\"sensor_id\":\"2000\",\"sensor_type\":\"sh\", \"timestamp\":\"%ld\", \"value\":\"%.2lf\"}", t, rand_float[rand_num]);

    sendto(sockfd, data, strlen(data),0, (struct sockaddr*)&servaddr, sizeof(servaddr));
    sleep(1);
  }
}
