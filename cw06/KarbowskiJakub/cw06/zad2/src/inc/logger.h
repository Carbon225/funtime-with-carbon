#ifndef JK_06_01_LOGGER_H
#define JK_06_01_LOGGER_H

void log_init();

void log_stop(int client_id);

void log_list(int client_id);

void log_2all(int sender_id, const char *body);

void log_2one(int sender_id, int recipient_id, const char *body);

#endif
