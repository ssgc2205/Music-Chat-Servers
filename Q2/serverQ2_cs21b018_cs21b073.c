#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUFFER_SIZE 1024 // Maximum size of the buffer
char* BASE_DIR;

void * client_func(void * soc)
{
    char buffer[BUFFER_SIZE] = {0};
    int client_soc = *(int *)soc;
    int r1 = recv(client_soc, buffer, BUFFER_SIZE, 0); // Receiving buffer from client socket connection
    if(r1 < 0)
    {
        perror("Not connected");
        exit(EXIT_FAILURE);
    }
    printf("%s\n", buffer);
    char operation[10]; // Either get or post according to the question
    char path[255]; // Path into the directory
    sscanf(buffer, "%s %s", operation, path); // Parsing the buffer and finding out operation and path
    if(strcmp(operation, "GET") == 0)
    {
        if(strcmp(path, "/") == 0) // if nothing is mentioned after the port address path should lead to index.html
        {
            strcpy(path, "/index.html");
        }
        char file_path[255];
        snprintf(file_path, sizeof(file_path), "%s%s", BASE_DIR, path); // Formatting into the file-path
        FILE* file = fopen(file_path, "rb"); // Open the file and read in binary
        if(file)
        {
            char* cnt_type = "text/plain"; // MIME types
            char* ext = strchr(path, '.');
            if(ext)
            {
                ext++;
                if (strcmp(ext, "html") == 0) 
                {
                    cnt_type = "text/html";
                } 
                else if (strcmp(ext, "css") == 0) 
                {
                    cnt_type = "text/css";
                } 
                else if (strcmp(ext, "js") == 0) 
                {
                    cnt_type = "application/javascript";
                } 
                else if (strcmp(ext, "jpeg") == 0 || strcmp(ext, "jpg") == 0) 
                {
                    cnt_type = "image/jpeg";
                } 
                else if (strcmp(ext, "png") == 0) 
                {
                    cnt_type = "image/png";
                }
            }
            char response[BUFFER_SIZE];
            fseek(file, 0, SEEK_END);
            long file_size = ftell(file);
            fseek(file, 0, SEEK_SET);
            sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n", cnt_type, file_size);
            send(client_soc, response, strlen(response), 0);
            while (!feof(file)) 
            {
                fread(buffer, 1, BUFFER_SIZE, file);
                send(client_soc, buffer, BUFFER_SIZE, 0);
            }
            fclose(file);
        }
        else // If an invalid address is given then path should lead to 404.html
        {
            char file_path2[255];
            snprintf(file_path2, sizeof(file_path2), "%s%s", BASE_DIR, "/404.html");
            FILE* file1 = fopen(file_path2, "rb");
            if(file1)
            {
                char* cnt_type = "text/plain";
                char* ext = strchr("/404.html", '.');
                if(ext)
                {
                    ext++;
                    if (strcmp(ext, "html") == 0) 
                    {
                        cnt_type = "text/html";
                    } 
                    else if (strcmp(ext, "css") == 0) 
                    {
                        cnt_type = "text/css";
                    } 
                    else if (strcmp(ext, "js") == 0) 
                    {
                        cnt_type = "application/javascript";
                    } 
                    else if (strcmp(ext, "jpeg") == 0 || strcmp(ext, "jpg") == 0) 
                    {
                        cnt_type = "image/jpeg";
                    } 
                    else if (strcmp(ext, "png") == 0) 
                    {
                        cnt_type = "image/png";
                    }
                }
                char response[BUFFER_SIZE];
                fseek(file1, 0, SEEK_END);
                long file_size = ftell(file1);
                fseek(file1, 0, SEEK_SET);
                sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n", cnt_type, file_size);
                send(client_soc, response, strlen(response), 0);
                while (!feof(file1)) 
                {
                    fread(buffer, 1, BUFFER_SIZE, file1);
                    send(client_soc, buffer, BUFFER_SIZE, 0);
                }
                fclose(file1);
            }
        }
    }
    else if(strcmp(operation, "POST") == 0) // If the operation is post
    {
        char response[] = "HTTP/1.1 200 OK\r\n\r\nPOST request received";
        send(client_soc, response, strlen(response), 0);
        char* post_data_start = strstr(buffer, "%**%");
        char* first = post_data_start + 4;
        char* last = NULL;
        while(post_data_start) // To count the number of characters, number of words and number of sentences
        {
            post_data_start += 4;
            post_data_start = strstr(post_data_start, "%**%");
            if(!post_data_start)
            {
                break;
            }
            last = post_data_start;
        }
        int length = last - first;
        char* data = (char*)malloc(length);
        strncpy(data, first, length);
        data[length] = '\0';
        int chars = 0;
        int words = 0;
        int sentns = 0;
        int in_word = 0;
        for(int i = 0; i < strlen(data); i++)
        {
            if(data[i] != ' ' && data[i] != '\n' && data[i] != '\t' && data[i] != '\r')
            {
                chars++;
            }
        }
        for(int i = 0; i < strlen(data); i++)
        {
            if(data[i] == ' ' || data[i] == '\n' || data[i] == '\t')
            {
                if(in_word == 1)
                {
                    words++;
                    in_word = 0;
                }
            }
            else
                {
                    in_word = 1;
                }
        }
        if(in_word == 1)
        {
            words++;
        }
        for(int i = 0; i < strlen(data); i++)
        {
            if(data[i] == '.' || data[i] == '!')
            {
                sentns++;
            }
        }
        char response1[BUFFER_SIZE];
        sprintf(response1, "HTTP/1.1 200 OK\r\n\r\nCharacters: %d\r\nWords: %d\r\nSentences: %d\r\n\r\n", chars, words, sentns);
        send(client_soc, response1, strlen(response1), 0);
        printf("characters - %d\n", chars);
        printf("words - %d\n", words);
        printf("sentences - %d\n", sentns);
        printf("%s\n", data);
    }
    else
    {
        char response[] = "HTTP/1.1 405 Method Not Allowed\r\n\r\n405 Method Not Allowed";
        send(client_soc, response, strlen(response), 0);
    }
    close(client_soc);
    pthread_exit(NULL);
}

int main(int argc, char* argv[])
{
    //strcpy(BASE_DIR, "./webroot");
    if(argc < 3) // If either port number or directory is not specified then print an error
    {
        perror("Insufficient arguments");
        exit(EXIT_FAILURE);
    }
    BASE_DIR = (char*)malloc(strlen(argv[2])); // Directory
    strcpy(BASE_DIR, argv[2]);
    int port = atoi(argv[1]);
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t clen = sizeof(client_addr);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    int serv_soc = socket(AF_INET, SOCK_STREAM, 0);
    if(serv_soc < 0)
    {
        perror("No Socket created");
        exit(EXIT_FAILURE);
    }
    int bi = bind(serv_soc, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if(bi < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    int li = listen(serv_soc, 10);
    if(li < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    printf("Server is now listening on port %d\n", port);
    while(1)
    {
        pthread_t th1;
        int client_soc = accept(serv_soc, (struct sockaddr *)&client_addr, &clen);
        if(client_soc < 0)
        {
            perror("Not accepted");
            exit(EXIT_FAILURE);
            continue;
        }
        int cr = pthread_create(&th1, NULL, client_func, (void *)&client_soc); // Creating a thread and passing client socket as an argument for the funtion on which this thread runs
        if(cr != 0)
        {
            perror("Thread not created");
            exit(EXIT_FAILURE);
            continue;
        }
    }
    close(serv_soc);
    return 0;
}