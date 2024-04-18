/*
 * Generated by util/mkerr.pl DO NOT EDIT
 * Copyright 1995-2019 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#ifndef HEADER_BUFERR_H
# define HEADER_BUFERR_H

# ifndef HEADER_SYMHACKS_H
#  include "symhacks.h"
# endif

# ifdef  __cplusplus
extern "C"
# endif
int ERR_load_BUF_strings(void);

/*
 * BUF function codes.
 */
# define BUF_F_BUF_MEM_GROW                               100
# define BUF_F_BUF_MEM_GROW_CLEAN                         105
# define BUF_F_BUF_MEM_NEW                                101

/*
 * BUF reason codes.
 */

#endif