#include <iostream>
#include <stdio.h>
#include <cstdio>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctime>
#include <sys/time.h>
#include <cstring>
#include <string>
#include <unistd.h>
#define DATA 1
#define ACK 2
#define MESSAGE_LEN 2000
using namespace std;

struct frame{
	int event_type;	//DATA or ACK
	int seq_num;
	int ack_num;
	char data[MESSAGE_LEN];
};