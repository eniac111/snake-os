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

static char rcsid[]="$Id: SMBPasswdGen.c,v 1.2 1997/05/22 11:33:22 williams Exp $";


#include <string.h>
#include <stdio.h>
#include "nt.h"

void SambaPassGen(char *pass,char ascii_p16[66])
{

  int i;	 
  fstring         new_passwd;
  uchar           new_p16[16];
  uchar           new_nt_p16[16];

  new_passwd[0] = '\0';
  strncpy(new_passwd, pass, sizeof(fstring));
  
  memset(new_nt_p16, '\0', 16);
  E_md4hash((uchar *) new_passwd, new_nt_p16);
  
  /* Mangle the passwords into Lanman format */
  new_passwd[14] = '\0';
  strupper(new_passwd);
  
  /*
   * Calculate the SMB (lanman) hash functions of both old and new passwords.
   */
    
  memset(new_p16, '\0', 16);
  E_P16((uchar *) new_passwd, new_p16);
  
  /*
   * If we get here either we were root or the old password checked out
   * ok.
   */
  /* Create the 32 byte representation of the new p16 */
  for (i = 0; i < 16; i++) {
    sprintf(&ascii_p16[i * 2], "%02X", (uchar) new_p16[i]);
  }

  /* Add on the NT md4 hash */
  ascii_p16[32] = ':';
  for (i = 0; i < 16; i++) {
      sprintf(&ascii_p16[(i * 2)+33], "%02X", (uchar) new_nt_p16[i]);
  }

  ascii_p16[65]='\0'; 
}

