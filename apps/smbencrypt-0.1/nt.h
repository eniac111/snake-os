/*
 * Copyright (c) 1997 Nigel Williams <williams@dcs.qmw.ac.uk>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#ifndef SMBNT4_H
#define SMBNT4_H

#include <string.h>
#define StrnCpy(A,B,C) strncpy(A,B,C)

#define strupper(A) strupr(A)

typedef unsigned char uchar;
typedef short int16;
typedef unsigned short uint16;
typedef long int32;
typedef unsigned long uint32;
typedef char fstring[128];

#ifdef WIN32
enum {false,true};
typedef int bool;
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef HAVE_STRUPR

#include <ctype.h>

static char *strupr(char *s)
{
  char *p=s;
  while((*p=toupper(*p)))p++;
  return s;
}

#endif

  void str_to_key(uchar *str,uchar *key);
  void D1(uchar *k, uchar *d, uchar *out);
  void E1(uchar *k, uchar *d, uchar *out);
  void E_P16(uchar *p14,uchar *p16);
  void E_P24(uchar *p21, uchar *c8, uchar *p24);
  void SMBencrypt(uchar *passwd, uchar *c8, uchar *p24);
  void E_md4hash(uchar *passwd, uchar *p16);
  void SMBNTencrypt(uchar *passwd, uchar *c8, uchar *p24);
  void SambaPassGen(char *pass,char ascii_p16[66]);

#ifdef __cplusplus
};
#endif



#endif

