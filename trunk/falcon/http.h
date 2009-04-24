/* -*- mode: C; c-basic-offset: 8; indent-tabs-mode: nil; tab-width: 8 -*- */

#include "url.h"
#include "buffer.h"

#ifndef HTTP_H
#define HTTP_H

int http_transfer(UrlResource *, buffer_t*);

#endif /* HTTP_H */

