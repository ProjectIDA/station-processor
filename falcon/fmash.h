#ifndef FMASH_H
#define FMASH_H

#include <buffer.h>
#include <falcon.h>
#include <falcon_structures.h>
#include <simclist.h>
#include <stdint.h>
#include <time.h>

#include <arpa/inet.h>

/* =====-----------------------------------------============================ */
/* ==== FMash Compression/Decompression Functions =========================== */
/* =====-----------------------------------------============================ */
int fmash_csv_to_msh( csv_buffer_t* csv, uint8_t** raw_msh, size_t* length );
int fmash_msh_to_csv( csv_buffer_t** csv, uint8_t* raw_msh, size_t length );

#endif

