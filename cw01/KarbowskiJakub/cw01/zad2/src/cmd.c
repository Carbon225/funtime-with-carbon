#include "cmd.h"

#include <string.h>

cmd_t cmd_parse(const char *cmd)
{
    if (!strncmp(cmd, CMD_CREATE_TABLE_STR, sizeof(CMD_CREATE_TABLE_STR)))
        return CMD_CREATE_TABLE;

    if (!strncmp(cmd, CMD_WC_FILES_STR, sizeof(CMD_WC_FILES_STR)))
        return CMD_WC_FILES;

    if (!strncmp(cmd, CMD_REMOVE_BLOCK_STR, sizeof(CMD_REMOVE_BLOCK_STR)))
        return CMD_REMOVE_BLOCK;

    return CMD_INVALID;
}
