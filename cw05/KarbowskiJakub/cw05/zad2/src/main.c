#include <stdio.h>
#include <string.h>

static const char HELP[] =
        "SO Lab5 Zad2 - Jakub Karbowski\n"
        "Usage:\n"
        "%s nadawca|data            - print emails\n"
        "%s ADDRESS SUBJECT MESSAGE - send email\n";

static int read_mail_sender()
{
    FILE *f = popen("mail -N | sort -k3", "w");
    if (!f)
    {
        fprintf(stderr, "Could open mail\n");
        return -1;
    }

    fprintf(f, "f*\nx\n");

    pclose(f);

    return 0;
}

static int read_mail_date()
{
    FILE *f = popen("mail -N | nl | sort -nr | cut -f2-", "w");
    if (!f)
    {
        fprintf(stderr, "Could open mail\n");
        return -1;
    }

    fprintf(f, "f*\nx\n");

    pclose(f);

    return 0;
}

static int send_mail(const char *to, const char *sub, const char *msg)
{
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "mail -s '%s' %s", sub, to);
    FILE *f = popen(cmd, "w");
    if (!f)
    {
        fprintf(stderr, "Could open mail\n");
        return -1;
    }

    fprintf(f, "%s\n", msg);

    pclose(f);

    return 0;
}

int main(int argc, char** argv)
{
    if (argc == 2)
    {
        if (!strcmp("nadawca", argv[1]))
        {
            return read_mail_sender();
        }
        else if (!strcmp("data", argv[1]))
        {
            return read_mail_date();
        }
    }
    else if (argc == 4)
    {
        return send_mail(argv[1], argv[2], argv[3]);
    }

    fprintf(stderr, HELP, argv[0], argv[0]);
    return -1;
}
