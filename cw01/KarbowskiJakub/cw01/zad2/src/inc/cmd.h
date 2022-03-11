#ifndef JK_ZAD2_SRC_INC_CMD_H
#define JK_ZAD2_SRC_INC_CMD_H

typedef enum cmd_t
{
    CMD_INVALID,
    CMD_CREATE_TABLE,
    CMD_WC_FILES,
    CMD_REMOVE_BLOCK,
} cmd_t;

static const char CMD_CREATE_TABLE_STR[] = "create_table";
static const char CMD_WC_FILES_STR[] = "wc_files";
static const char CMD_REMOVE_BLOCK_STR[] = "remove_block";

cmd_t cmd_parse(const char *cmd);

#endif
