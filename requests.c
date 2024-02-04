#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

// Functia construieste un mesaj de tipul GET
char *compute_get_request(char *host, char *url, char *command, char *cookie, char *token, char *jsonData,
                            int jsonDataSize)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    if (cookie == NULL) {
     return NULL;  
    }

    if (!strcmp(command,"get")) {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    if (!strcmp(command,"delete")) {
        sprintf(line, "DELETE %s HTTP/1.1", url);
    }

    compute_message(message, line);
    memset(line,0,LINELEN);

    // Step 2: add the host
    strcat(line,"Host: ");
    strcat(line,host);
    compute_message(message,line);
    memset(line,0,LINELEN);

    // Step 3: set cookie field
    if(cookie != NULL){
        strcat(line,"Cookie: connect.sid=");
        strcat(line,cookie);
        compute_message(message,line);
        memset(line,0,LINELEN);
    }

    // Step 4: set token field
    if(token != NULL){
        strcat(line,"Authorization: Bearer: ");
        strcat(line,token);
        compute_message(message,line);
        memset(line,0,LINELEN);
    }

    if(jsonData != NULL){
        strcat(line,"Authorization: Bearer: ");
        strcat(line,token);
        compute_message(message,line);
        memset(line,0,LINELEN);

        strcat(line,"Content-Type: ");
        strcat(line,"application/json");
        compute_message(message,line);
        memset(line,0,LINELEN);

        sprintf(line,"Content-Length: %d", jsonDataSize);
        compute_message(message,line);
        memset(line, 0, LINELEN);
        compute_message(message,line);

        strcat(message,jsonData);
        memset(line, 0, LINELEN);
    }

    // Step 5: add final new line
    compute_message(message, "");
    return message;
}

// Functia construieste un mesaj de tipul POST
char *compute_post_request(char *host, char *url, char* content_type, char *jsonData,
                            int jsonDataSize, char *cookie, char *token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    // char *body_data_buffer = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    memset(line,0,LINELEN);

    // Step 2: add the host
    strcat(line,"Host: ");
    strcat(line,host);
    compute_message(message,line);
    memset(line,0,LINELEN);

   // Step 3: set cookie field
    if(cookie != NULL){
        strcat(line,"Cookie: connect.sid=");
        strcat(line,cookie);
        compute_message(message,line);
        memset(line,0,LINELEN);
    }

    // Step 4: set token field
    if(token != NULL){
        strcat(line,"Authorization: Bearer: ");
        strcat(line,token);
        compute_message(message,line);
        memset(line,0,LINELEN);
    }

    strcat(line,"Content-Type: ");
    strcat(line,content_type);
    compute_message(message,line);
    memset(line,0,LINELEN);

    sprintf(line,"Content-Length: %d", jsonDataSize);
    compute_message(message,line);
    memset(line, 0, LINELEN);
    compute_message(message,line);

    strcat(message,jsonData);
    memset(line, 0, LINELEN);

    free(line);
    return message;
}
