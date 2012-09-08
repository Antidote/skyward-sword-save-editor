// Copyright 2007,2008  Segher Boessenkool  <segher@kernel.crashing.org>
// Licensed under the terms of the GNU GPL, version 2
// http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt

#include "tools.h"

#include <stddef.h>	// to accommodate certain broken versions of openssl
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//
// basic data types
//

u16 be16(const u8 *p)
{
	return (p[0] << 8) | p[1];
}

u32 be32(const u8 *p)
{
	return (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
}

u64 be64(const u8 *p)
{
	return ((u64)be32(p) << 32) | be32(p + 4);
}

u64 be34(const u8 *p)
{
	return 4 * (u64)be32(p);
}

void wbe16(u8 *p, u16 x)
{
	p[0] = x >> 8;
	p[1] = x;
}

void wbe32(u8 *p, u32 x)
{
	wbe16(p, x >> 16);
	wbe16(p + 2, x);
}

void wbe64(u8 *p, u64 x)
{
	wbe32(p, x >> 32);
	wbe32(p + 4, x);
}

//
// error handling
//

void fatal(const char *s, ...)
{
	char message[256];
	va_list ap;

	va_start(ap, s);
	vsnprintf(message, sizeof message, s, ap);

	perror(message);

	exit(1);
}

//
// output formatting
//

void print_bytes(u8 *x, u32 n)
{
	u32 i;

	for (i = 0; i < n; i++)
		fprintf(stderr, "%02x", x[i]);
}

void hexdump(u8 *x, u32 n)
{
	u32 i, j;

	for (i = 0; i < n; i += 16) {
		fprintf(stderr, "%04x:", i);
		for (j = 0; j < 16 && i + j < n; j++) {
			if ((j & 3) == 0)
				fprintf(stderr, " ");
			fprintf(stderr, "%02x", *x++);
		}
		fprintf(stderr, "\n");
	}
}

void dump_tmd(u8 *tmd)
{
	u32 i, n;
	u8 *p;

	printf("       issuer: %s\n", tmd + 0x140);
    printf("  sys_version: %016llx\n", be64(tmd + 0x0184));
    printf("     title_id: %016llx\n", be64(tmd + 0x018c));
	printf("   title_type: %08x\n", be32(tmd + 0x0194));
	printf("     group_id: %04x\n", be16(tmd + 0x0198));
	printf("title_version: %04x\n", be16(tmd + 0x01dc));
	printf(" num_contents: %04x\n", be16(tmd + 0x01de));
	printf("   boot_index: %04x\n", be16(tmd + 0x01e0));

	n = be16(tmd + 0x01de);
	p = tmd + 0x01e4;
	for (i = 0; i < n; i++) {
		printf("cid %08x  index %04x  type %04x  size %08llx\n",
		       be32(p), be16(p + 4), be16(p + 6), be64(p + 8));
		p += 0x24;
	}
}
