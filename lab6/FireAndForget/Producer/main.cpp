#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "zmq.h"
#include <unistd.h>
#include <pthread.h>
#include <string>
#include <map>

typedef struct MD {
	char message[128];
	int clientId;
	char name[128] {'\0'};
	int value = -1;
} MessageData;

const char* first_client = "tcp://127.0.0.1:4040";

void ZMQ_SEND(MessageData *md, void* socket_request) {
	zmq_msg_t zmqMessage;
	zmq_msg_init_size(&zmqMessage, sizeof(MessageData));
	memcpy(zmq_msg_data(&zmqMessage), md, sizeof(MessageData));
	int send = zmq_msg_send(&zmqMessage, socket_request, ZMQ_DONTWAIT);
	if (send == -1) perror("Zmq_msg_send");
	zmq_msg_close(&zmqMessage);
}

bool start = true;
std::map<int, unsigned int> times;

void* handler(void* socket_answers) {

	while (start) {
		zmq_msg_t message;
		zmq_msg_init(&message);
		int recv = zmq_msg_recv(&message, socket_answers, ZMQ_DONTWAIT);
		if (recv != -1) {
			MessageData *md = (MessageData *)zmq_msg_data(&message);
			if (strcmp(md->message, "heartbit") == 0) {
				times.insert_or_assign(md->clientId, time(NULL));
			} else {
				if (strcmp(md->name, "") == 0) {
					if (md->clientId != -1) {
						printf("%s%d\n", md->message, md->clientId);
					} else {
						printf("%s\n", md->message);
					}
				} else {
					printf("%s%d: '%s' not found\n", md->message, md->clientId, md->name);
				}
			}
		}
		zmq_msg_close(&message);
	}

	return NULL;
}

int main(int argc, char const *argv[])
{	
	void* context_answers = zmq_ctx_new(); // контекст для ответов
	void* context_request = NULL; // контекст для отправки запросов пользователя
	if (context_answers == NULL) perror("zmq_ctx_new");

	printf("Producer starting...\n");

	void* socket_answers = zmq_socket(context_answers, ZMQ_PULL); // инициализация сокета для ответов
	void* socket_request = NULL; // сокет для отправки запросов
	if (socket_answers == NULL) perror("zmq_socket");

	int bind_sa = zmq_bind(socket_answers, "tcp://127.0.0.1:4004"); // создание сокета для ответов
	if (bind_sa == -1) perror("zmq_bind");

	pthread_t hand;
	if (pthread_create(&hand, NULL, handler, socket_answers) != 0) perror("Поток не смог создаться");
	
	int child = -1;
	int last_heartbit_time = -1;

	for (;;)
	{
		MessageData md;
		int status;

		status = scanf("%s", md.message);
		if (status <= 0) {
			printf("Wrong enter main command\n");
			if (status == EOF) break;
			continue;
		}

		if (strcmp(md.message, "create") == 0) {

			status = scanf("%d", &md.clientId);
			if (status <= 0) {
				printf("Wrong enter in block of create\n");
				continue;
			}

			if (socket_request == NULL) { // если дерево пусто вообще, то создаем тут
				context_request = zmq_ctx_new();
				socket_request = zmq_socket(context_request, ZMQ_PUSH);
				char tcp[100] {'\0'};
				sprintf(tcp, "%s%d", "tcp://127.0.0.1:400", md.clientId);
				int bind_sr = zmq_bind(socket_request, tcp); // создание сокета для ответов
				if (bind_sr == -1) {
					perror("zmq_bind server");
					zmq_close(socket_request);
					zmq_ctx_destroy(context_request);
					context_request = NULL;
					socket_request = NULL;
					continue;
				}
				int id = fork();
				if (id == -1) {
					perror("fork error");
					continue;
				} else if (id == 0) {
					if (execl("../Consumer/a.out", tcp, (char*)&(md.clientId), NULL) == -1) {
						perror("execl");
						continue;
					}
				}
				child = md.clientId;
			} else { // иначе отправляем команду
				ZMQ_SEND(&md, socket_request);
			}

			//sleep(1);

		} else if (strcmp(md.message, "remove") == 0) {

			status = scanf("%d", &md.clientId);
			if (status <= 0) {
				printf("Wrong enter in block of remove\n");
				continue;
			}

			if (socket_request == NULL) {
 				printf("Error: Not found\n");
 				continue;
			} else {
				ZMQ_SEND(&md, socket_request);
				if (child == md.clientId) {
					ZMQ_SEND(&md, socket_request);
					zmq_close(socket_request);
					zmq_ctx_destroy(context_request);
					socket_request = NULL;
					context_request = NULL;
					child = -1;	
				} 
			}
			

		} else if (strcmp(md.message, "exec") == 0) {

			char endl1;
			status = scanf("%d%s%c", &md.clientId, md.name, &endl1);
			if (status <= 0 || status == 1) {
				printf("Wrong enter in block of exec\n");
				continue;
			} else if (endl1 == ' ') {
				status = scanf("%d", &md.value);
				if (status <= 0) {
					printf("Wrong enter in block of exec\n");
					continue;
				}
				
				if (socket_request == NULL) {
	 				printf("Error: Not found\n");
	 				continue;
				} else {
					ZMQ_SEND(&md, socket_request);
				}
			} else if (endl1 == '\n') {
				if (socket_request == NULL) {
	 				printf("Error: Not found\n");
	 				continue;
				} else {
					ZMQ_SEND(&md, socket_request);
				}
			}
		
		} else if (strcmp(md.message, "heartbit") == 0) {

			status = scanf("%d", &md.clientId);
			if (status <= 0) {
				printf("Wrong enter in block of heartbit\n");
				continue;
			}

			if (socket_request == NULL) {
	 			printf("Error: No nodes\n");
	 			continue;
			} else {
				ZMQ_SEND(&md, socket_request);
				last_heartbit_time = md.clientId;
			}
		} else if (strcmp(md.message, "ping") == 0) {

			int id; 
			status = scanf("%d", &id);
			if (status <= 0) {
				printf("Wrong enter in block of ping\n");
				continue;
			}

			auto it = times.find(id);
			if (it == times.end()) {
				printf("Ok: 0\n");
			} else {
				if ((time(NULL)-(it->second)) > last_heartbit_time * 4 ) {
					printf("Ok: 0\n");
				} else {
					printf("Ok: 1\n");
				}
			}

		} else {
			printf("Wrong command\n");
		}

	}

	if (socket_request != NULL)  {
		MessageData md;
		strcpy(md.message, "remove");
		md.clientId = child;
		ZMQ_SEND(&md, socket_request);
	}

	sleep(5);

	start = false;
	if (pthread_join(hand, NULL)) {
    	perror("Поток не завершился. Завершение программы...");
    }

	// We never get here though.
	zmq_close(socket_answers);
	zmq_ctx_destroy(context_answers);

	zmq_close(socket_request);
	zmq_ctx_destroy(context_request);

	return 1;
}


