#include <string.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <map>
#include "zmq.h"

typedef struct MD {
	char message[128];
	int clientId;
	char name[128] = {'\0'};
	int value = -1;
} MessageData;

bool ZMQ_SEND(MessageData *md, void* socket_request) {
	zmq_msg_t zmqMessage;
	zmq_msg_init_size(&zmqMessage, sizeof(MessageData));
	memcpy(zmq_msg_data(&zmqMessage), md, sizeof(MessageData));
	int send = zmq_msg_send(&zmqMessage, socket_request, ZMQ_DONTWAIT);
	if (send == -1) {
		perror("Zmq_msg_send error");
		zmq_msg_close(&zmqMessage);
		return false;
	}
	zmq_msg_close(&zmqMessage);
	return true;
}

bool start = true;
useconds_t timer = -1;
int id;

void* send_time(void* socket_answer) {
	
	while (start) {
		MessageData ok{"heartbit", id, "", id};
		ZMQ_SEND(&ok, socket_answer);
		usleep(timer);
	}
	return NULL;
}

int main(int argc, char const *argv[])
{	
	void* my_pull = zmq_ctx_new();
	void* context_push_to_parrent = zmq_ctx_new();
	void* context_left_child = NULL;
	void* context_right_child = NULL;
	if (my_pull == NULL || context_push_to_parrent == NULL) perror("zmq_ctx_new error");

	void* my_socket = zmq_socket(my_pull, ZMQ_PULL);
	void* socket_answer = zmq_socket(context_push_to_parrent, ZMQ_PUSH);
	void* socket_left_child = NULL;
	void* socket_right_child = NULL;
	if (my_socket == NULL || socket_answer == NULL) perror("zmq_socket error");
	
	int bind_ms = zmq_connect(my_socket, argv[0]);
	int bind_sa = zmq_connect(socket_answer, "tcp://127.0.0.1:4004");
	if (bind_ms == -1 || bind_sa == -1) perror("zmq_bind client error");

	id = *(argv[1]);
	int children[2] {-1, -1};

	std::map<std::string, int> slov;

	pthread_t heart;

	//printf("Ok: %d\n", getpid());
	MessageData ok{"Ok: ", getpid(), "", id};
	ZMQ_SEND(&ok, socket_answer);

	for (;;)
	{	

		zmq_msg_t message;
		zmq_msg_init(&message);
		zmq_msg_recv(&message, my_socket, 0);
		MessageData *md = (MessageData *)zmq_msg_data(&message);
		//printf("[%d] Message from producer: %s  clientId: %d name: %s value %d\n", id, md->message, md->clientId, md->name, md->value);
		zmq_msg_close(&message);

		if (strcmp(md->message, "create") == 0) {

			if (md->clientId == id) {
				//printf("Error: Already exists\n");
				MessageData send{"Error: Already exists", -1, "", id};
				ZMQ_SEND(&send, socket_answer);
			} else if (md->clientId > id) {
				if (context_right_child == NULL) {
 					context_right_child = zmq_ctx_new();
					socket_right_child = zmq_socket(context_right_child, ZMQ_PUSH);
					if (context_right_child == NULL || socket_right_child == NULL) {
						perror("right child error");
						continue;
					}
					char tcp[100] {'\0'};
					sprintf(tcp, "%s%d", "tcp://127.0.0.1:400", md->clientId);
					int bind_sr = zmq_bind(socket_right_child, tcp); // создание сокета для ответов
					if (bind_sr == -1) {
						perror("zmq_bind server");
						zmq_close(socket_right_child);
						zmq_ctx_destroy(context_right_child);
						context_right_child = NULL;
						socket_right_child = NULL; 
						continue;
					}
					int id = fork();
					if (id == -1) {
						perror("fork error");
						continue;
					} else if (id == 0) {
						if (execl("../Consumer/a.out", tcp, &md->clientId, NULL) == -1) {
							perror("execl");
							continue;
						}
					}
					children[1] = md->clientId;

				} else {
					if (!ZMQ_SEND(md, socket_right_child)) {
						zmq_close(socket_right_child);
						zmq_ctx_destroy(context_right_child);
						context_right_child = NULL;
						socket_right_child = NULL;
						children[1] = -1;
					}
				}
			} else {
				if (context_left_child == NULL) {
 					context_left_child = zmq_ctx_new();
					socket_left_child = zmq_socket(context_left_child, ZMQ_PUSH);
					if (context_left_child == NULL || socket_left_child == NULL) {
						perror("right child error");
						continue;
					}
					char tcp[100] {'\0'};
					sprintf(tcp, "%s%d", "tcp://127.0.0.1:400", md->clientId);
					int bind_sr = zmq_bind(socket_left_child, tcp); // создание сокета для ответов
					if (bind_sr == -1) {
						perror("zmq_bind server error");
						zmq_close(socket_left_child);
						zmq_ctx_destroy(context_left_child);
						context_left_child = NULL;
						socket_left_child = NULL;
						continue;
					}
					int id = fork();
					if (id == -1) {
						perror("fork error");
						continue;
					} else if (id == 0) {
						if (execl("../Consumer/a.out", tcp, &md->clientId, NULL) == -1) {
							perror("execl error");
							continue;
						}
					}
					children[0] = md->clientId;
				} else {
					//ZMQ_SEND(md, socket_left_child);
					if (!ZMQ_SEND(md, socket_left_child)) {
						zmq_close(socket_left_child);
						zmq_ctx_destroy(context_left_child);
						context_left_child = NULL;
						socket_left_child = NULL;
						children[0] = -1;
					}
				}
			}
			//printf("[%d] left - %d, right - %d\n", id, children[0], children[1]);

		} else if (strcmp(md->message, "remove") == 0) {

			if (md->clientId == id) {
				if (children[0] != -1) {
					md->clientId = children[0];
					ZMQ_SEND(md, socket_left_child);
				}
				if (children[1] != -1) {
					md->clientId = children[1];
					ZMQ_SEND(md, socket_right_child);
				}
				//printf("Ok\n");
				MessageData send{"Ok", -1, "", id};
				ZMQ_SEND(&send, socket_answer);
				break;
			} else if (md->clientId > id) {
				if (context_right_child == NULL) {
 					//printf("Error: Not found\n");
 					MessageData send{"Error: Not found", -1, "", id};
					ZMQ_SEND(&send, socket_answer);
				} else {
					if (!ZMQ_SEND(md, socket_right_child)) {
						zmq_close(socket_right_child);
						zmq_ctx_destroy(context_right_child);
						context_right_child = NULL;
						socket_right_child = NULL;
						children[1] = -1;
					}
					if (children[1] == md->clientId) {
						zmq_close(socket_right_child);
						zmq_ctx_destroy(context_right_child);
						socket_right_child = NULL;
						context_right_child = NULL;
						children[1] = -1;
					}
				}
			} else {
				if (context_left_child == NULL) {
 					//printf("Error: Not found\n");
 					MessageData send{"Error: Not found", -1, "", id};
					ZMQ_SEND(&send, socket_answer);
				} else {
					if (!ZMQ_SEND(md, socket_left_child)) {
						zmq_close(socket_left_child);
						zmq_ctx_destroy(context_left_child);
						context_left_child = NULL;
						socket_left_child = NULL;
						children[0] = -1;
					}
					if (children[0] == md->clientId) {
						zmq_close(socket_left_child);
						zmq_ctx_destroy(context_left_child);
						socket_left_child = NULL;
						context_left_child = NULL;
						children[0] = -1;
					}
				}
			}

		} else if (strcmp(md->message, "exec") == 0) {

			if (md->clientId == id) {
				if (md->value == -1) {
					auto it = slov.find(std::string(md->name));
					if (it == slov.end()) {
						//printf("Ok:%d: %s not found\n", id, md->name);
						MessageData send{"Ok:", id, "", id};
						//send.name = md->name;
						strcpy(send.name, md->name);
						ZMQ_SEND(&send, socket_answer);
					} else {
						//printf("Ok:%d\n", it->second);
						MessageData send{"Ok:", it->second, "", id};
						ZMQ_SEND(&send, socket_answer);
					}
				} else {
					slov.insert(std::pair<std::string, int>(std::string(md->name), md->value));
					//printf("Ok:%d\n", id);
					MessageData send{"Ok:", id, "", id};
					ZMQ_SEND(&send, socket_answer);
				}
			} else if (md->clientId > id) {
				if (context_right_child == NULL) {
					//printf("Error: Not found\n");
					MessageData send{"Error: Not found", -1, "", id};
					ZMQ_SEND(&send, socket_answer);
				} else {
					if (!ZMQ_SEND(md, socket_right_child)) {
						zmq_close(socket_right_child);
						zmq_ctx_destroy(context_right_child);
						context_right_child = NULL;
						socket_right_child = NULL;
						children[1] = -1;
					}
				}
			} else {
				if (context_left_child == NULL) {
					//printf("Error: Not found\n");
					MessageData send{"Error: Not found", -1, "", id};
					ZMQ_SEND(&send, socket_answer);
				} else {
					if (!ZMQ_SEND(md, socket_left_child)) {
						zmq_close(socket_left_child);
						zmq_ctx_destroy(context_left_child);
						context_left_child = NULL;
						socket_left_child = NULL;
						children[0] = -1;
					}
				}
			}
		
		} else if (strcmp(md->message, "heartbit") == 0) {
			if (children[0] != -1) {
				if (!ZMQ_SEND(md, socket_left_child)) {
					zmq_close(socket_left_child);
					zmq_ctx_destroy(context_left_child);
					context_left_child = NULL;
					socket_left_child = NULL;
					children[0] = -1;
				}
			}
			if (children[1] != -1) {
				if (!ZMQ_SEND(md, socket_right_child)) {
					zmq_close(socket_right_child);
					zmq_ctx_destroy(context_right_child);
					context_right_child = NULL;
					socket_right_child = NULL;
					children[1] = -1;
				}
			}

			timer = md->clientId;
			start = false;
			sleep(1);
			start = true;
			if (pthread_create(&heart, NULL, send_time, socket_answer) != 0) perror("Поток не смог создаться");
			MessageData send{"Ok", -1, "", id};
			ZMQ_SEND(&send, socket_answer);

			continue;
			
		}
	}

	
	zmq_close(my_socket);
	zmq_ctx_destroy(my_pull);
	zmq_close(socket_answer);
	zmq_ctx_destroy(context_push_to_parrent);
	zmq_close(socket_left_child);
	zmq_ctx_destroy(context_left_child );
	zmq_close(socket_right_child);
	zmq_ctx_destroy(context_right_child);

	start = false;
	pthread_join(heart, NULL);

	return 0;
}
