/*
 * This file is compile into meter's bin file and use for qfix to
 * generate mutiple .bin file to support different platform.
 */
typedef struct {
    const int id;
    const char *name;
}board_config_type;

static const board_config_type board_config[] = {
{0x0000,"transmitter_meter"},
{0x1000,"watch_meter"}
};
