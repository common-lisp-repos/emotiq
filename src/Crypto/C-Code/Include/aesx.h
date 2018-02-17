/**
 * \file aesx.h
 *
 *  Copyright (C) 2006-2010, Brainspark B.V.
 *
 *  This file is part of PolarSSL (http://www.polarssl.org)
 *  Lead Maintainer: Paul Bakker <polarssl_maintainer at polarssl.org>
 *
 *  All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef POLARSSL_AESX_H
#define POLARSSL_AESX_H

#define AESX_ENCRYPT     1
#define AESX_DECRYPT     0

#define POLARSSL_ERR_AESX_INVALID_KEY_LENGTH                 -0x0800
#define POLARSSL_ERR_AESX_INVALID_INPUT_LENGTH               -0x0810

#include "my_types.h"

/**
 * \brief          AES context structure
 */
typedef struct
{
    int     nr;           /*!<  number of rounds  */
    UInt32 *rk;           /*!<  AES round keys    */
    UInt32  buf[128];      /*!<  unaligned data    */
}
aesx_context;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          AES key schedule (encryption)
 *
 * \param ctx      AES context to be initialized
 * \param key      encryption key
 * \param keysize  must be 128, 192 or 256
 *
 * \return         0 if successful, or POLARSSL_ERR_AESX_INVALID_KEY_LENGTH
 */
int aesx_setkey_enc( aesx_context *ctx, const unsigned char *key, int keysize );

/**
 * \brief          AES key schedule (decryption)
 *
 * \param ctx      AES context to be initialized
 * \param key      decryption key
 * \param keysize  must be 128, 192 or 256
 *
 * \return         0 if successful, or POLARSSL_ERR_AESX_INVALID_KEY_LENGTH
 */
int aesx_setkey_dec( aesx_context *ctx, const unsigned char *key, int keysize );

/**
 * \brief          AES-ECB block encryption/decryption
 *
 * \param ctx      AES context
 * \param mode     AESX_ENCRYPT or AESX_DECRYPT
 * \param input    16-byte input block
 * \param output   16-byte output block
 *
 * \return         0 if successful
 */
int aesx_crypt_ecb( aesx_context *ctx,
                    int mode,
                    const unsigned char input[16],
                    unsigned char output[16] );

/**
 * \brief          AES-CBC buffer encryption/decryption
 *                 Length should be a multiple of the block
 *                 size (16 bytes)
 *
 * \param ctx      AES context
 * \param mode     AESX_ENCRYPT or AESX_DECRYPT
 * \param length   length of the input data
 * \param iv       initialization vector (updated after use)
 * \param input    buffer holding the input data
 * \param output   buffer holding the output data
 *
 * \return         0 if successful, or POLARSSL_ERR_AESX_INVALID_INPUT_LENGTH
 */
int aesx_crypt_cbc( aesx_context *ctx,
                    int mode,
                    int length,
                    unsigned char iv[16],
                    const unsigned char *input,
                    unsigned char *output );

/**
 * \brief          AES-CFB128 buffer encryption/decryption.
 *
 * \param ctx      AES context
 * \param mode     AESX_ENCRYPT or AESX_DECRYPT
 * \param length   length of the input data
 * \param iv_off   offset in IV (updated after use)
 * \param iv       initialization vector (updated after use)
 * \param input    buffer holding the input data
 * \param output   buffer holding the output data
 *
 * \return         0 if successful
 */
int aesx_crypt_cfb128( aesx_context *ctx,
                       int mode,
                       int length,
                       int *iv_off,
                       unsigned char iv[16],
                       const unsigned char *input,
                       unsigned char *output );

/**
 * \brief          Checkup routine
 *
 * \return         0 if successful, or 1 if the test failed
 */
int aesx_self_test( int verbose );

#ifdef __cplusplus
}
#endif

#endif /* aesx.h */
