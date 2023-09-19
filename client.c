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
#include <ctype.h>

void serialize_user(char **string, char *username, char *password) {
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    char *serialized_string = NULL;
    json_object_set_string(root_object, "username", username);
    json_object_set_string(root_object, "password", password);
    serialized_string = json_serialize_to_string_pretty(root_value);
    string[0] = malloc(strlen(serialized_string) + 1);
    strcpy(string[0], serialized_string);
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
}

void serialize_book(char **string, char *title, char *author, char *genre, char *publisher, int page_count) {
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    char *serialized_string = NULL;
    json_object_set_string(root_object, "title", title);
    json_object_set_string(root_object, "author", author);
    json_object_set_string(root_object, "genre", genre);
    json_object_set_string(root_object, "publisher", publisher);
    json_object_set_number(root_object, "page_count", page_count);
    serialized_string = json_serialize_to_string_pretty(root_value);
    string[0] = malloc(strlen(serialized_string) + 1);
    strcpy(string[0], serialized_string);
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
}

int isNumber(char *buffer) {
    if (buffer[0] == '-') {
        return 0;
    }
    for (int i = 0; i < strlen(buffer); i++) {
        if (!isdigit(buffer[i])) {
            return 0;
        }
    }
    return 1;
}

void free_buffers(char *message, char *response) {
    if (message != NULL) {
        free(message);
    }
    if (response != NULL) {
        free(response);
    }
}

int wrong_string(char *string) {
    if (strcmp(string, "")) {
        return 0;
    } else if (string[0] == ' ') {
        return 0;
    }
    return 1;
}

int main(int argc, char *argv[])
{
    char *message;
    char *response;
    int sockfd;

    char buffer[1024];
    char **cookies = malloc(sizeof(char*) * 10);
    for (int i = 0; i < 10; i ++) {
        cookies[i] = malloc(200);
    }
    int cookies_count = 0;
    char *token = NULL;

    while (1) {
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strlen(buffer) - 1] = '\0';
        if (strcmp(buffer, "register") == 0) {
            if (cookies_count > 0) {
                printf("You're logged in! Logout before you register!\n");
                continue;
            }
            sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
            char username[1024], password[1024];
            // input pentru username
            printf("username=");
            fgets(username, sizeof(username), stdin);
            username[strlen(username) - 1] = '\0';
            // input pentru parola
            printf("password=");
            fgets(password, sizeof(password), stdin);
            password[strlen(password) - 1] = '\0';
            // verific ca username-ul sau parola sa nu contina spatii
            if (strstr(username, " ") || strstr(password, " ")) {
                printf("Unsuccessful. Don't use spaces for your username or for your password! Try again!\n");
                continue;
            } else if (wrong_string(username) || wrong_string(password)) {
                printf("Invalid username or password. Try again!\n");
                continue;
            }
            char **serialized_string = malloc(sizeof(char*));
            serialize_user(serialized_string, username, password);

            message = compute_post_request("34.254.242.81:8080", "/api/v1/tema/auth/register", "application/json", serialized_string, 1, NULL, 0, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            // Printez un mesaj corespunzator in functie de eroare/succes
            if (strstr(response, "\"error\":\"The username")) {
                printf("400 - Bad Request - Username already used! Please change it up a bit!\n");
            } else if (strstr(response, "HTTP/1.1 2")) {
                printf("201 - Created - Succesfully registered!\n");
            }
            free(serialized_string[0]);
            free(serialized_string);
            free_buffers(message, response);
            close_connection(sockfd);
        } else if (strcmp(buffer, "login") == 0) {
            if (cookies_count > 0) {
                printf("You're already logged in! If you want to login to another account, logout first!\n");
                continue;
            }
            sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
            char username[1024], password[1024];
            // input pentru username
            printf("username=");
            fgets(username, sizeof(username), stdin);
            username[strlen(username) - 1] = '\0';
            // input pentru parola
            printf("password=");
            fgets(password, sizeof(password), stdin);
            password[strlen(password) - 1] = '\0';
            // verific ca username-ul sau parola sa nu contina spatii
            if (strstr(username, " ") || strstr(password, " ")) {
                printf("Unsuccessful. Don't use spaces for your username or for your password! Try again!\n");
                continue;
            } else if (wrong_string(username) || wrong_string(password)) {
                printf("Invalid username or password. Try again!\n");
                continue;
            }

            char **serialized_string = malloc(sizeof(char*));
            serialize_user(serialized_string, username, password);

            message = compute_post_request("34.254.242.81:8080", "/api/v1/tema/auth/login", "application/json", serialized_string, 1, NULL, 0, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            // Preiau cookie-urile primite in vectorul cookies
            char *p = strstr(response, "Set-Cookie: ");
            while (p != NULL) {
                char *q = strstr(p, ";");
                strncpy(cookies[cookies_count], p + 12, q - p - 12);
                cookies_count++;
                p = strstr(p + 1, "Set-Cookie: ");
            }
            // Printez un mesaj corespunzator in functie de eroare/succes
            if (strstr(response, "No account with this username!")) {
                printf("400 - Bad Request - There is no account with this username! Try again or register first!\n");
            } else if (strstr(response, "Credentials are not good")) {
                printf("400 - Bad Request - Credentials are not good! Wrong password.\n");
            } else if(strstr(response, "HTTP/1.1 2")) {
                printf("200 - OK - Succesfully logged in!\n");
            }

            free(serialized_string[0]);
            free(serialized_string);
            free_buffers(message, response);
            close_connection(sockfd);
        } else if (strcmp(buffer, "enter_library") == 0) {
            sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
            message = compute_get_request("34.254.242.81:8080", "/api/v1/tema/library/access", NULL, cookies, cookies_count, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            // Daca este totul ok, luam token-ul si printam mesaj de succes,
            // altfel anunt utilizatorul ca nu este logat.
            if (strstr(response, "HTTP/1.1 2")) {
                char *p = strstr(response, "token");
                char *q = strstr(p + 8, "\"");
                token = malloc(1024);
                strncpy(token, p + 8, q - p - 8);
                printf("200 - OK - Succesfully entered library!\n");
            } else if (strstr(response, "error")) {
                printf("401 - Unauthorized - You are not logged in!\n");
            }
            free_buffers(message, response);
            close_connection(sockfd);
        } else if (strcmp(buffer, "get_books") == 0) {
            sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
            message = compute_get_request("34.254.242.81:8080", "/api/v1/tema/library/books", NULL, cookies, cookies_count, token);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            // Printez un mesaj corespunzator in functie de eroare/succes
            if (strstr(response, "Authorization header is missing")) {
                printf("403 - Forbidden - You don't have access to the library!\n");
            } else if (strstr(response, "Error when decoding token")) {
                printf("500 - Internal Server Error - Error when decoding token. You don't have access to the library!\n");
            } else if (strstr(response, "HTTP/1.1 2")) {
                if (strstr(response, "[{")) {
                    printf("200 - OK - Here are the books from the library!\n");
                    // Parsez Json-ul din response ca sa aiba un aspect mai placut
                    JSON_Value *json_value = json_parse_string(strstr(response, "[{"));
                    char *books = json_serialize_to_string_pretty(json_value);
                    puts(books);
                } else {
                    printf("200 - OK - You currently have no books in the library!\n");
                }
            }
            free_buffers(message, response);
            close_connection(sockfd);
        } else if (strcmp(buffer, "get_book") == 0) {
            printf("id=");
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strlen(buffer) - 1] = '\0';
            if (!isNumber(buffer) || wrong_string(buffer)) {
                printf("The id should be a number! Try again!\n");
                continue;
            }
            // Creez url-ul potrivit
            char *url = malloc(1024);
            strcpy(url, "/api/v1/tema/library/books/");
            strcat(url, buffer);

            sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
            message = compute_get_request("34.254.242.81:8080", url, NULL, cookies, cookies_count, token);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            // Printez un mesaj corespunzator in functie de eroare/succes
            if (strstr(response, "Authorization header is missing!")) {
                printf("403 - Forbidden - You don't have access to the library\n");
            } else if (strstr(response, "No book was found!")) {
                printf("404 - Not Found - There's no book with that id!\n");
            } else if (strstr(response, "Error when decoding token")){
                printf("500 - Internal Server Error - Error when decoding token.\n");
            } else if (strstr(response, "HTTP/1.1 2")) {
                printf("200 - OK - Here is your book!\n");
                // Parsez Json-ul din response ca sa aiba un aspect mai placut
                JSON_Value *json_value = json_parse_string(strstr(response, "{"));
                char *book = json_serialize_to_string_pretty(json_value);
                puts(book);
            }
            close_connection(sockfd);
            free_buffers(message, response);
            free(url);
        } else if (strcmp(buffer, "add_book") == 0) {
            char title[1024], author[1024], genre[1024], publisher[1024];
            // Preiau datele de la tastatura si verific corectitudinea
            printf("title=");
            fgets(title, sizeof(title), stdin);
            title[strlen(title) - 1] = '\0';
            printf("author=");
            fgets(author, sizeof(author), stdin);
            author[strlen(author) - 1] = '\0';
            printf("genre=");
            fgets(genre, sizeof(genre), stdin);
            genre[strlen(genre) - 1] = '\0';
            printf("publisher=");
            fgets(publisher, sizeof(publisher), stdin);
            publisher[strlen(publisher) - 1] = '\0';
            printf("page_count=");
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strlen(buffer) - 1] = '\0';
            // verific daca string-urile sunt valide
            if (wrong_string(title) || wrong_string(author) || wrong_string(genre) || wrong_string(publisher)) {
                printf("You have to type something for every section. Also, don't start with a space.\n");
                continue;
            }
            // verific ca page_count sa fie un numar pozitiv
            if (!isNumber(buffer) || wrong_string(buffer)) {
                printf("Please enter a number for page_count! Try again!\n");
                continue;
            }

            char **serialized_string = malloc(sizeof(char*));
            serialize_book(serialized_string, title, author, genre, publisher, atoi(buffer));

            sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
            message = compute_post_request("34.254.242.81:8080", "/api/v1/tema/library/books", "application/json", serialized_string, 1, NULL, 0, token);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            // Printez un mesaj corespunzator in functie de eroare/succes
            if (strstr(response, "Error when decoding token")) {
                printf("500 - Internal Server Error - Error when decoding token.\n");
            } else if (strstr(response, "Authorization header is missing!")) {
                printf("403 - Forbidden - You don't have access to the library\n");
            } else if (strstr(response, "HTTP/1.1 2")) {
                printf("200 - OK - Book succesfully added!\n");
            }
            close_connection(sockfd);            
            free_buffers(message, response);
        } else if (strcmp(buffer, "delete_book") == 0) {
            printf("id=");
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strlen(buffer) - 1] = '\0';
            if (!isNumber(buffer) || wrong_string(buffer)) {
                printf("The id should be a number! Try again!\n");
                continue;
            }
            char *url = malloc(1024);
            strcpy(url, "/api/v1/tema/library/books/");
            strcat(url, buffer);

            sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
            message = compute_delete_request("34.254.242.81:8080", url, NULL, NULL, 0, token);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            // Printez un mesaj corespunzator in functie de eroare/succes
            if (strstr(response, "Authorization header is missing!")) {
                printf("403 - Forbidden - You don't have access to the library\n");
            } else if (strstr(response, "No book was deleted!")) {
                printf("404 - Not Found - There is no book with that id. No book was deleted!\n");
            } else if (strstr(response, "HTTP/1.1 2")) {
                printf("200 - OK - Succesfully deleted book with ID %s!\n", buffer);
            } else if (strstr(response, "Error when decoding token")) {
                printf("500 - Internal Server Error - Error when decoding token.\n");
            }
            free(url);
            free_buffers(message, response);
            close_connection(sockfd);
        } else if (strcmp(buffer, "logout") == 0) {
            sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
            message = compute_get_request("34.254.242.81:8080", "/api/v1/tema/auth/logout", NULL, cookies, cookies_count, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            // Golim vectorul de cookies si token-ul.
            if (token != NULL) {
                memset(token, '\0', strlen(token));
                free(token);
                token = NULL;
            }
            if (cookies_count != 0) {
                for(int i = 0; i < cookies_count; i++) {
                    memset(cookies[i], 0, strlen(cookies[i]));
                }
                cookies_count = 0;
            }
            // Printez un mesaj corespunzator in functie de eroare/succes
            if (strstr(response, "error")) {
                printf("400 - Bad request - You are not logged in!\n");
            } else if (strstr(response, "HTTP/1.1 2")) {
                printf("200 - OK - You succesfully logged out!\n");
            }
            close_connection(sockfd);
            free_buffers(message, response);
        } else if (strcmp(buffer, "exit") == 0) {
            break;
        } else {
           printf("Unrecognised command! Available commands:\n\t- register\n\t- login\n\t- enter_library\n\t- get_books\n\t");
           printf("- get_book\n\t- add_book\n\t- delete_book\n\t- logout\n\t- exit\n");
        }
    }
    // free the allocated data at the end!
    if(token != NULL) {
        free(token);
    }
    for (int i = 0; i < 10; i ++) {
        free(cookies[i]);
    }
    free(cookies);
    return 0;
}
