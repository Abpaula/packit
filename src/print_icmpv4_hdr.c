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

#include "../include/packit.h"
#include "../include/capture.h"
#include "../include/utils.h"
#include "../include/error.h"

void
print_icmpv4_hdr(u_int8_t *packet)
{
    u_int8_t *s_addr, *d_addr;
    u_int8_t *icmp_t, *icmp_c;
    
    struct libnet_icmpv4_hdr *icmphdr;

#ifdef DEBUG
    fprintf(stdout, "DEBUG: print_icmpv4_hdr()\n"); 
#endif

#ifndef icmp_iphdr
#define icmp_iphdr icmphdr->dun.ip.idi_ip
#endif

    icmphdr = malloc(sizeof(struct libnet_icmpv4_hdr));
    memset(icmphdr, 0, sizeof(struct libnet_icmpv4_hdr));

    icmphdr = (struct libnet_icmpv4_hdr *)(packet + IPV4_H + hdr_len);

    s_addr = (u_int8_t *)malloc(sizeof(s_addr));
    d_addr = (u_int8_t *)malloc(sizeof(d_addr));

    icmp_t = retrieve_icmp_type(icmphdr->icmp_type);
    icmp_c = retrieve_icmp_code(icmphdr->icmp_type, icmphdr->icmp_code);

    fprintf(stdout, "ICMP header: Type: %s(%d)  ", icmp_t, icmphdr->icmp_type);

    switch(icmphdr->icmp_type) 
    {
        case ICMP_REDIRECT: 
            fprintf(stdout, "Code: %s(%d)  Gateway: %s  ", 
                icmp_c, icmphdr->icmp_code, 
                libnet_addr2name4(ntohl(icmphdr->hun.gateway), 0));

            if(verbose)
            {
                s_addr = libnet_addr2name4(icmp_iphdr.ip_src.s_addr, 0);
                d_addr = libnet_addr2name4(icmp_iphdr.ip_dst.s_addr, 0);

                fprintf(stdout, "\n");
                fprintf(stdout, "\t     Original IP header:\n");
                fprintf(stdout, "\t     Src Address: %s  Dst Address: %s\n", s_addr, d_addr);
                fprintf(stdout, "\t     TTL: %d  ID: %d  TOS: 0x%X  Len: %d  ",
                    icmp_iphdr.ip_ttl,
                    ntohs(icmp_iphdr.ip_id),
                    icmp_iphdr.ip_tos,
                    ntohs(icmp_iphdr.ip_len));

                if(ntohs(icmp_iphdr.ip_off) & IP_DF)
                    fprintf(stdout, "(DF)");
            }

            break;

        case ICMP_UNREACH: 
            fprintf(stdout, "Code: %s(%d)  ", icmp_c, icmphdr->icmp_code);

            if(icmphdr->icmp_code == ICMP_UNREACH_NEEDFRAG)
                fprintf(stdout, "MTU: %d  Pad: %d  ", icmphdr->hun.frag.mtu, icmphdr->hun.frag.pad); 

            if(verbose)
            {
                s_addr = libnet_addr2name4(icmp_iphdr.ip_src.s_addr, 0);
                d_addr = libnet_addr2name4(icmp_iphdr.ip_dst.s_addr, 0);

                fprintf(stdout, "\n");
                fprintf(stdout, "\t     Original IP header:\n");
                fprintf(stdout, "\t     Src Address: %s  Dst Address: %s\n", s_addr, d_addr);
                fprintf(stdout, "\t     TTL: %d  ID: %d  TOS: 0x%X  Len: %d  ",
                    icmp_iphdr.ip_ttl,
                    ntohs(icmp_iphdr.ip_id),
                    icmp_iphdr.ip_tos,
                    ntohs(icmp_iphdr.ip_len));

                if(ntohs(icmp_iphdr.ip_off) & IP_DF)
                    fprintf(stdout, "(DF)");
            }

            break;

        case ICMP_TIMXCEED: case ICMP_PARAMPROB:
            fprintf(stdout, "Code: %s(%d)  ", icmp_c, icmphdr->icmp_code);

            if(verbose)
            {
                s_addr = libnet_addr2name4(icmp_iphdr.ip_src.s_addr, 0);
                d_addr = libnet_addr2name4(icmp_iphdr.ip_dst.s_addr, 0);

                fprintf(stdout, "\n");
                fprintf(stdout, "\t     Original IP header:\n");
                fprintf(stdout, "\t     Src Address: %s  Dst Address: %s\n", s_addr, d_addr);
                fprintf(stdout, "\t     TTL: %d  ID: %d  TOS: 0x%X  Len: %d  ",
                    icmp_iphdr.ip_ttl,
                    ntohs(icmp_iphdr.ip_id),
                    icmp_iphdr.ip_tos,
                    ntohs(icmp_iphdr.ip_len));

                if(ntohs(icmp_iphdr.ip_off) & IP_DF)
                    fprintf(stdout, "(DF)");
            }

            break;

        case ICMP_TSTAMPREPLY:
            if(icmphdr->icmp_code > 0)
                fprintf(stdout, "Code: %s(%d)  ", icmp_c, icmphdr->icmp_code);

            fprintf(stdout, "ID: %d  Seqn: %d\n", icmphdr->icmp_id, icmphdr->icmp_seq);
            fprintf(stdout, "\t     Original: %lu  Received: %lu  Transmit: %lu", 
                (unsigned long)ntohl(icmphdr->icmp_otime), 
                (unsigned long)ntohl(icmphdr->icmp_rtime), 
                (unsigned long)ntohl(icmphdr->icmp_ttime));    

            break;

        case ICMP_MASKREPLY:
            if(icmphdr->icmp_code > 0)
                fprintf(stdout, "Code: %s(%d)  ", icmp_c, icmphdr->icmp_code);

            fprintf(stdout, "ID: %d  Seqn: %d\n", icmphdr->icmp_id, icmphdr->icmp_seq);
            fprintf(stdout, "\t     Mask: %s  ", libnet_addr2name4(ntohl(icmphdr->dun.mask), 0));
            break;

        case ICMP_ECHOREPLY: case ICMP_ECHO: 
        case ICMP_MASKREQ: case ICMP_TSTAMP:
            if(icmphdr->icmp_code > 0)
                fprintf(stdout, "Code: %s(%d)  ", icmp_c, icmphdr->icmp_code);

            fprintf(stdout, "ID: %d  Seqn: %d", icmphdr->icmp_id, icmphdr->icmp_seq);
	    break;

        case ICMP_TRACEROUTE:
            if(icmphdr->icmp_code > 0)
                fprintf(stdout, "Code: %s(%d)  ", icmp_c, icmphdr->icmp_code);

            break;

    }
    
    fprintf(stdout, "\n");

    return;
}
