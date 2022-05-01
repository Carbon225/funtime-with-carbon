#include "logger.h"

#include <stdio.h>
#include <time.h>

void log_init()
{
    FILE *f = fopen("server.log", "a");
    time_t t = time(NULL);
    fprintf(f, "[INIT] | %s", ctime(&t));
    fclose(f);
}

void log_stop(int client_id)
{
    FILE *f = fopen("server.log", "a");
    time_t t = time(NULL);
    fprintf(f, "[STOP] Client: %d | %s", client_id, ctime(&t));
    fclose(f);
}

void log_list(int client_id)
{
    FILE *f = fopen("server.log", "a");
    time_t t = time(NULL);
    fprintf(f, "[LIST] Client: %d | %s", client_id, ctime(&t));
    fclose(f);
}

void log_2all(int sender_id, const char *body)
{
    FILE *f = fopen("server.log", "a");
    time_t t = time(NULL);
    fprintf(f, "[2ALL] Sender: %d, Message: %s | %s", sender_id, body, ctime(&t));
    fclose(f);
}

void log_2one(int sender_id, int recipient_id, const char *body)
{
    FILE *f = fopen("server.log", "a");
    time_t t = time(NULL);
    fprintf(f, "[2ONE] Sender: %d, Recipient: %d, Message: %s | %s", sender_id, recipient_id, body, ctime(&t));
    fclose(f);
}
