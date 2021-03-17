#ifndef _FYMODEM_H_
#define _FYMODEM_H_

/**
 * Free YModem implementation.
 *
 * Fredrik Hederstierna 2014
 *
 * This file is in the public domain.
 * You can do whatever you want with it.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

/* max length of filename */
#define FYMODEM_FILE_NAME_MAX_LENGTH  (64)


typedef enum {
    YMODEM_ERROR_DEVICE_OFFLINE  = -3,
    YMODEM_ERROR_WRONG_FILE_NAME = -2,
    YMODEM_ERROR_GENERNAL        = -1,
    YMODEM_ERROR_NOERROR         = 0,
} ymodem_error_type;


/* receive file over ymodem */
int32_t fymodem_receive(uint8_t *rxdata,
                        size_t rxsize,
                        char filename[FYMODEM_FILE_NAME_MAX_LENGTH]);

/* send file over ymodem */
int32_t fymodem_send(uint8_t *txdata,
                     size_t txsize,
                     const char *filename);

#endif
