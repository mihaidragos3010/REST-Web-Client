#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

#define LEN_COOKIE 100

int main(int argc, char *argv[])
{
    char* sendMessage;
    char* receiveMessage;
    char* cookie = NULL;
    char* token = NULL;
    int serverSocket;
    char serverIpAddress[16] = "34.254.242.81";
    int port = 8080;
    char command[100];

    while(1){

        fgets(command,50,stdin);

        if(!strcmp(command,"register\n")){
            char username[100];
            char password[100];
            JSON_Value *clientValue = json_value_init_object();
            JSON_Object *clientStats = json_value_get_object(clientValue);

            serverSocket = open_connection(serverIpAddress,port,AF_INET,SOCK_STREAM,0);

            printf("username= ");
            fgets(username,100,stdin);
            username[strlen(username)-1] = '\0';

            printf("password= ");
            fgets(password,100,stdin);
            password[strlen(password)-1] = '\0';
            json_object_set_string(clientStats,"password",password);
            
            if(!strcmp(username,"\0") || !strcmp(password,"\0")){
                printf("Server: Credentials cannot be blank!!\n\n");
                continue;
            }

            if(strstr(username," ") != NULL){
                printf("Server: Incorect format!!!\n\n");
                continue;
            }
            if(strstr(password," ") != NULL){
                printf("Server: Incorect format!!!\n\n");
                continue;
            }
            
            json_object_set_string(clientStats,"username",username);
            json_object_set_string(clientStats,"password",password);
            char* serialized_string = json_serialize_to_string(clientValue);
            
            sendMessage = compute_post_request(serverIpAddress,(char *)"/api/v1/tema/auth/register",(char *)"application/json", 
                                        serialized_string, strlen(serialized_string), cookie, token);

            send_to_server(serverSocket, sendMessage);

            receiveMessage = receive_from_server(serverSocket);
            
            printOutPutCommand(receiveMessage,"register");
                
            close(serverSocket);
        }

        if(!strcmp(command,"login\n")){

            if(cookie != NULL){
                printf("You are already logged in!!!\n\n");
                continue;
            }

            char username[100];
            char password[100];
            JSON_Value *clientValue = json_value_init_object();
            JSON_Object *clientStats = json_value_get_object(clientValue);

            serverSocket = open_connection(serverIpAddress,port,AF_INET,SOCK_STREAM,0);

            printf("username= ");
            fgets(username,100,stdin);
            username[strlen(username)-1] = '\0';

            printf("password= ");
            fgets(password,100,stdin);
            password[strlen(password)-1] = '\0';
            json_object_set_string(clientStats,"password",password);
            
            if(!strcmp(username,"\0") || !strcmp(password,"\0")){
                printf("Server: Credentials cannot be blank!!\n\n");
                continue;
            }

            if(strstr(username," ") != NULL){
                printf("Server: Incorect format!!!\n\n");
                continue;
            }
            if(strstr(password," ") != NULL){
                printf("Server: Incorect format!!!\n\n");
                continue;
            }
            
            json_object_set_string(clientStats,"username",username);
            json_object_set_string(clientStats,"password",password);
            char* serialized_string = json_serialize_to_string(clientValue);
            
            sendMessage = compute_post_request(serverIpAddress,(char *)"/api/v1/tema/auth/login",(char *)"application/json", 
                                        serialized_string, strlen(serialized_string), cookie, token);

            send_to_server(serverSocket, sendMessage);
            
            receiveMessage = receive_from_server(serverSocket);

            cookie = printOutPutCommand(receiveMessage,"login");

        }

        if(!strcmp(command,"enter_library\n")){
            serverSocket = open_connection(serverIpAddress,port,AF_INET,SOCK_STREAM,0); // de intrebat la lab!!!
            sendMessage = compute_get_request(serverIpAddress,(char *)"/api/v1/tema/library/access","get", cookie, token, NULL, 0);

            if(sendMessage == NULL){
                printf("Server: Need to log in to access library!!\n\n");
                continue;
            }

            send_to_server(serverSocket, sendMessage);

            receiveMessage = receive_from_server(serverSocket);

            token = printOutPutCommand(receiveMessage,"enter_library");
        }

        if(!strcmp(command,"get_books\n")){
            serverSocket = open_connection(serverIpAddress,port,AF_INET,SOCK_STREAM,0);
            if(token == NULL){
                printf("Server: Need to enter to library before to show all books!!!\n\n");
            }else{
                sendMessage = compute_get_request(serverIpAddress,(char *)"/api/v1/tema/library/books","get", cookie, token, NULL, 0);

                send_to_server(serverSocket, sendMessage);
                receiveMessage = receive_from_server(serverSocket);

                 printOutPutCommand(receiveMessage,"get_books");
            }

        }
        
        if(!strcmp(command,"get_book\n")){
            serverSocket = open_connection(serverIpAddress,port,AF_INET,SOCK_STREAM,0);
            JSON_Value *clientValue = json_value_init_object();
            JSON_Object *clientStats = json_value_get_object(clientValue);
            if(token == NULL){
                printf("Need to enter to library before to show a book!!!\n\n");
            }else{
                char content[100];
                memset(content,0,100);
                printf("id=");
                fgets(content,100,stdin);
                if(checkIsContentInteger(content) == 0){
                    printf("Need to choose an integer value !!\n\n");
                    continue;
                }

                content[strlen(content)-1] = '\0';
                json_object_set_string(clientStats,"id",content);

                char* serialized_string = json_serialize_to_string(clientValue);
                
                char path[100];
                memset(path,0,100);
                strcat(path,"/api/v1/tema/library/books/");
                strcat(path,content);
                sendMessage = compute_get_request(serverIpAddress,path, 
                                                    "get", cookie, token, 
                                                    serialized_string, strlen(serialized_string));

                send_to_server(serverSocket, sendMessage);

                receiveMessage = receive_from_server(serverSocket);

                printOutPutCommand(receiveMessage,"get_book");
            }
        }

        if(!strcmp(command,"add_book\n")){
            serverSocket = open_connection(serverIpAddress,port,AF_INET,SOCK_STREAM,0);
            JSON_Value *clientValue = json_value_init_object();
            JSON_Object *clientStats = json_value_get_object(clientValue);

            if(token == NULL){
                printf("Server: Need to enter to library before to add a book!!!\n\n");
            }else{

                char content[100];
                int ok = 1; //0 need to write a string/ all good
                printf("title=");
                fgets(content,100,stdin);
                if(!strcmp(content,"\n")){
                    ok=0;
                }
                content[strlen(content)-1] = '\0';
                json_object_set_string(clientStats,"title",content);

                printf("author=");
                fgets(content,100,stdin);
                if(!strcmp(content,"\n")){
                    ok=0;
                }
                content[strlen(content)-1] = '\0';
                json_object_set_string(clientStats,"author",content);

                printf("genre=");
                fgets(content,100,stdin);
                if(!strcmp(content,"\n")){
                    ok=0;
                }
                content[strlen(content)-1] = '\0';
                json_object_set_string(clientStats,"genre",content);

                printf("publisher=");
                fgets(content,100,stdin);
                if(!strcmp(content,"\n")){
                    ok=0;
                }
                content[strlen(content)-1] = '\0';
                json_object_set_string(clientStats,"publisher",content);

                printf("page_count=");
                fgets(content,100,stdin);
                if(checkIsContentInteger(content) == 0){
                    printf("Server: Need to choose an integer value !!\n\n");
                    continue;
                }
                content[strlen(content)-1] = '\0';
                json_object_set_string(clientStats,"page_count",content);
                
                if(ok == 0){
                    printf("Server: You must to write a string\n\n");
                    continue;
                }
                char* serialized_string = json_serialize_to_string(clientValue);

                sendMessage = compute_post_request(serverIpAddress,(char *)"/api/v1/tema/library/books",(char *)"application/json", 
                                        serialized_string, strlen(serialized_string), cookie, token);

                send_to_server(serverSocket, sendMessage);

                receiveMessage = receive_from_server(serverSocket);

                printf("Server: Add book successful!!\n\n");
            }
        }

        if(!strcmp(command,"delete_book\n")){

            serverSocket = open_connection(serverIpAddress,port,AF_INET,SOCK_STREAM,0);
            JSON_Value *clientValue = json_value_init_object();
            JSON_Object *clientStats = json_value_get_object(clientValue);
            if(token == NULL){
                printf("Need to enter to library before to delete a book!!!\n\n");
            }else{
                char content[100];
                memset(content,0,100);
                printf("id=");
                fgets(content,100,stdin);
                if(checkIsContentInteger(content) == 0){
                    printf("Need to choose an integer value !!\n\n");
                    continue;
                }
                content[strlen(content)-1] = '\0';
                json_object_set_string(clientStats,"id",content);

                char* serialized_string = json_serialize_to_string(clientValue);
                
                char path[100];
                memset(path,0,100);
                strcat(path,"/api/v1/tema/library/books/");
                strcat(path,content);
                sendMessage = compute_get_request(serverIpAddress,path, 
                                                    "delete", cookie, token, 
                                                    serialized_string, strlen(serialized_string));

                send_to_server(serverSocket, sendMessage);

                receiveMessage = receive_from_server(serverSocket);

                printOutPutCommand(receiveMessage,"delete_book");
            }

        }

        if(!strcmp(command,"logout\n")){
            if(cookie == NULL){
                printf("Server: You need to be logged in!!\n\n");
            }else{
                
            serverSocket = open_connection(serverIpAddress,port,AF_INET,SOCK_STREAM,0); // de intrebat la lab!!!
            sendMessage = compute_get_request(serverIpAddress,(char *)"/api/v1/tema/auth/logout","get", cookie, token, NULL, 0);

            send_to_server(serverSocket, sendMessage);
            receiveMessage = receive_from_server(serverSocket);

            printOutPutCommand(receiveMessage,"logout");
            cookie = NULL;
            token = NULL;
            
            }
        }

        if(!strcmp(command,"exit\n")){
            break;
        }

        
    }

    return 0;
}
