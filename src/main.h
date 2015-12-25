/*
 * author: Darren Bounds <dbounds@intrusense.com>
 * copyright: Copyright (C) 2002 by Darren Bounds
 * license: This software is under GPL version 2 of license
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * packit official page at http://packit.sourceforge.net
 */

#ifndef __MAIN_H
#define __MAIN_H

#include <libnet.h>
#include <pcap.h>
#include <sys/types.h>
#include <net/bpf.h>
#include <signal.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "globals.h"
#include "init.h"
#include "define_defaults.h"
#include "injection.h"
#include "inject_defs.h"
#include "capture.h"
#include "capture_defs.h"

#define OPT_MAXLEN 32 

u_int32_t opt;
char *optarg;

void parse_capture_options(int, char *[]);
void parse_inject_options(int, char *[], u_int16_t);

#endif /* __MAIN_H */
