/*
 * This file is part of the TREZOR project.
 *
 * Copyright (C) 2014 Pavol Rusnak <stick@satoshilabs.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UTIL_H
#define UTIL_H

/* === Includes ============================================================ */

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
/* === Functions =========================================================== */

// read protobuf integer and advance pointer
uint32_t readprotobufint(uint8_t **ptr);

typedef uint32_t pb_size_t;
typedef uint8_t pb_byte_t;

int size_of_bytes(int str_len);
uint8_t strtohex(char c);
int hex2byte_arr(const char *buf, int len, uint8_t *out, int outbuf_size);
void int8_to_char(uint8_t *buffer, int len, char *out);

uint32_t convertEndian(uint32_t value);

#ifdef __cplusplus
}
#endif

#endif
