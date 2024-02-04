#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "buffer.h"
#include "parson.h"


#define HEADER_TERMINATOR "\r\n\r\n"
#define HEADER_TERMINATOR_SIZE (sizeof(HEADER_TERMINATOR) - 1)
#define CONTENT_LENGTH "Content-Length: "
#define CONTENT_LENGTH_SIZE (sizeof(CONTENT_LENGTH) - 1)

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int checkIsContentInteger(char *content){
    
    if(strlen(content)==1){
        return 0;
    }

    for(int i=0; i<strlen(content)-1;i++){
        if(!(content[i]>='0' && content[i]<='9')){
            return 0;
        }
    }
    return 1;
}

// Functia returneaza un mesaj de eroare in cazul in care am primit de la server un mesaj de eroare
// Functia trimite un string spre afisare care poate sa contina cookie-ul sau token-ul in cazul in care comanda elibereaza o astfel de valoare
char* printOutPutCommand(char *receiveMessage, char *command){
    char *commandMessage;

    if(!strcmp(command,"register")){
        commandMessage = strstr(receiveMessage, ":\"");
        if(commandMessage != NULL){
            memcpy(commandMessage,commandMessage+2,strlen(commandMessage));
            memset(commandMessage+strlen(commandMessage)-2,0,2);
            printf("Server: %s\n\n", commandMessage);
        }else{
            printf("Server: Register account successful!!\n\n");
        }
    }

    if(!strcmp(command,"login")){

        commandMessage = strstr(receiveMessage, "Set-Cookie: connect.sid=");
        if(commandMessage != NULL){
            commandMessage = strtok(commandMessage,"=");
            commandMessage = strtok(NULL,"=");
            commandMessage = strtok(commandMessage,";");
            printf("Server: %s\n\n", commandMessage);
        }else{
            commandMessage = strstr(receiveMessage, ":\"");
            memcpy(commandMessage,commandMessage+2,strlen(commandMessage));
            memset(commandMessage+strlen(commandMessage)-2,0,2);
            memcpy(commandMessage+strlen(commandMessage),"\n",1);
            printf("Server: %s\n", commandMessage);
            return NULL;
        }
    }

    if(!strcmp(command,"enter_library")){

        commandMessage = strstr(receiveMessage, ":\"");
        if(commandMessage != NULL){
            memcpy(commandMessage,commandMessage+2,strlen(commandMessage));
            memset(commandMessage+strlen(commandMessage)-2,0,2);
            printf("Server: %s\n\n", commandMessage);
        }else{
            commandMessage = strstr(receiveMessage, ":\"");
            memcpy(commandMessage,commandMessage+2,strlen(commandMessage));
            memset(commandMessage+strlen(commandMessage)-2,0,2);
            memcpy(commandMessage+strlen(commandMessage),"\n",1);
            printf("Server: %s\n", commandMessage);
            return NULL;
        }
    }

    // if(!strcmp(command,"get_books")){
    //     commandMessage = strstr(receiveMessage, "[");
    //     printf("com = %s\n", commandMessage);
    //     printf("Server: \n");

    //     JSON_Value *jsonValue = json_parse_string(commandMessage);
    //     printf("%s\n\n",json_serialize_to_string_pretty(jsonValue));

    // }
    if(!strcmp(command,"get_books")){
        commandMessage = strstr(receiveMessage, "[");
        // printf("s = %s\n", commandMessage);
        JSON_Value *jsonValue = json_parse_string_with_comments(commandMessage);
        JSON_Array *jsonArray = json_value_get_array(jsonValue);
        size_t jsonArraySize = json_array_get_count(jsonArray);
        printf("Server: \n");
        for (size_t index = 0; index < jsonArraySize; index++) {
            JSON_Object *jsonObject = json_array_get_object(jsonArray, index);
            int id = json_object_get_number(jsonObject, "id");
            const char *title = json_object_get_string(jsonObject, "title");
            printf("id=%d, title=%s\n", id, title);
        }

        printf("\n");
    }

    if(!strcmp(command,"get_book")){
        commandMessage = strstr(receiveMessage, "{");
        if(!strncmp("{\"id\":",commandMessage,6)){
            printf("Server: \n");
            JSON_Value *jsonValue = json_parse_string(commandMessage);
            JSON_Object *jsonObject = json_value_get_object(jsonValue);
            int id = json_object_get_number(jsonObject, "id");
            const char *title = json_object_get_string(jsonObject, "title");
            const char *author = json_object_get_string(jsonObject, "author");
            const char *genre = json_object_get_string(jsonObject, "genre");
            const char *publisher = json_object_get_string(jsonObject, "publisher");
            int page_count = json_object_get_number(jsonObject, "page_count");
            printf("id=%d\ntitle=%s\nauthor=%s\ngenre=%s\npublisher=%s\npage_count=%d\n\n", id, title, author, genre, publisher, page_count);
        }

        if(!strncmp("{\"error\":",commandMessage,8)){
            commandMessage = strstr(commandMessage,":");
            memcpy(commandMessage,commandMessage+2,strlen(commandMessage));
            memset(commandMessage+strlen(commandMessage)-2,0,2);
            memcpy(commandMessage+strlen(commandMessage),"\n",1);
            printf("Server: %s\n", commandMessage);
            return NULL;
        }

    }

    if(!strcmp(command,"delete_book")){
        commandMessage = strstr(receiveMessage, "{");
        
        if(commandMessage != NULL){
            if(!strncmp("{\"error\":",commandMessage,8)){
                commandMessage = strstr(commandMessage,":");
                memcpy(commandMessage,commandMessage+2,strlen(commandMessage));
                memset(commandMessage+strlen(commandMessage)-2,0,2);
                memcpy(commandMessage+strlen(commandMessage),"\n",1);
                printf("Server: %s\n", commandMessage);
                return NULL;
            }
        }
        
        printf("Server: Delete book successful!!\n\n");

    }

    if(!strcmp(command,"logout")){
        commandMessage = strstr(receiveMessage, "{");
        if(commandMessage != NULL){
            if(!strncmp("{\"error\":",commandMessage,8)){
                commandMessage = strstr(commandMessage,":");
                memcpy(commandMessage,commandMessage+2,strlen(commandMessage));
                memset(commandMessage+strlen(commandMessage)-2,0,2);
                memcpy(commandMessage+strlen(commandMessage),"\n",1);
                printf("Server: %s\n\n", commandMessage);
                return NULL;
            }
        }
        printf("Server: Logout successful!!\n\n");

    }

    return commandMessage;
    
}

void compute_message(char *message, const char *line)
{
    strcat(message, line);
    strcat(message, "\r\n");
}

int open_connection(char *host_ip, int portno, int ip_type, int socket_type, int flag)
{
    struct sockaddr_in serv_addr;
    int sockfd = socket(ip_type, socket_type, flag);
    if (sockfd < 0)
        error("ERROR opening socket");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = ip_type;
    serv_addr.sin_port = htons(portno);
    inet_aton(host_ip, &serv_addr.sin_addr);
    
    /* connect the socket */
    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    return sockfd;
}

void close_connection(int sockfd)
{
    close(sockfd);
}

void send_to_server(int sockfd, char *message)
{
    int bytes, sent = 0;
    int total = strlen(message);

    do
    {
        bytes = write(sockfd, message + sent, total - sent);
        if (bytes < 0) {
            error("ERROR writing message to socket");
        }

        if (bytes == 0) {
            break;
        }

        sent += bytes;
    } while (sent < total);
}

char *receive_from_server(int sockfd)
{
    char response[BUFLEN];
    buffer buffer = buffer_init();
    int header_end = 0;
    int content_length = 0;

    do {
        int bytes = read(sockfd, response, BUFLEN);

        if (bytes < 0){
            error("ERROR reading response from socket");
        }

        if (bytes == 0) {
            break;
        }

        buffer_add(&buffer, response, (size_t) bytes);
        
        header_end = buffer_find(&buffer, HEADER_TERMINATOR, HEADER_TERMINATOR_SIZE);

        if (header_end >= 0) {
            header_end += HEADER_TERMINATOR_SIZE;
            
            int content_length_start = buffer_find_insensitive(&buffer, CONTENT_LENGTH, CONTENT_LENGTH_SIZE);
            
            if (content_length_start < 0) {
                continue;           
            }

            content_length_start += CONTENT_LENGTH_SIZE;
            content_length = strtol(buffer.data + content_length_start, NULL, 10);
            break;
        }
    } while (1);
    size_t total = content_length + (size_t) header_end;
    
    while (buffer.size < total) {
        int bytes = read(sockfd, response, BUFLEN);

        if (bytes < 0) {
            error("ERROR reading response from socket");
        }

        if (bytes == 0) {
            break;
        }

        buffer_add(&buffer, response, (size_t) bytes);
    }
    buffer_add(&buffer, "", 1);
    return buffer.data;
}

char *basic_extract_json_response(char *str)
{   char *convertMessage;
    convertMessage = strstr(str, "{\"");
    printf("==%s\n", convertMessage);
    convertMessage = strtok(convertMessage,"\"");
    return convertMessage;
}
