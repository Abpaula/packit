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
#include "../include/inject.h"
#include "../include/utils.h"
#include "../include/error.h"

libnet_t *
shape_icmpv4_hdr(libnet_t *pkt_d)
{
    u_int8_t ih_payload[8];
    u_int32_t ih_payload_len = 8;
    u_int32_t ihn_gw, ihn_mask = 0;
    u_int32_t ihn_saddr, ihn_daddr;
    u_int32_t orig_hlen = IPV4_H + ICMPV4_H;

#ifdef DEBUG
    fprintf(stdout, "DEBUG: shape_icmpv4_hdr(): type: %d  code: %d\n", i4hdr_o.type, i4hdr_o.code);
#endif

    switch(i4hdr_o.type)
    {
        case ICMP_ECHOREPLY: case ICMP_ECHO: default:
#ifdef DEBUG
            fprintf(stdout, "DEBUG: Building ICMP echo header\n");
#endif

            if(i4hdr_o.rand_seqn)
                i4hdr_o.seqn = (unsigned short)retrieve_rand_int(P_UINT16);

            if(i4hdr_o.rand_id)
                i4hdr_o.id = (unsigned short)retrieve_rand_int(P_UINT16);

            hdr_len = ICMPV4_ECHO_H;

            if(libnet_build_icmpv4_echo(
	        i4hdr_o.type,
	        i4hdr_o.code,
	        0,
	        i4hdr_o.id,
	        i4hdr_o.seqn,
	        payload,
	        payload_len,
	        pkt_d,
	        0) == -1)
	    {
	        fatal_error("Unable to build ICMPv4 echo header: %s", libnet_geterror(pkt_d));
	    }

            break;

	case ICMP_UNREACH: case ICMP_REDIRECT: case ICMP_TIMXCEED:
            if(i4hdr_o.orig_p == IPPROTO_TCP)
	        orig_hlen = IPV4_H + TCP_H;
	    else if(i4hdr_o.orig_p == IPPROTO_UDP)
                orig_hlen = IPV4_H + UDP_H;
            else if(i4hdr_o.orig_p == IPPROTO_ICMP)
	        orig_hlen = IPV4_H + ICMPV4_H;

	    if(i4hdr_o.rand_orig_s_addr)
                i4hdr_o.orig_s_addr = retrieve_rand_ipv4_addr();

            if(i4hdr_o.rand_orig_d_addr)
		i4hdr_o.orig_d_addr = retrieve_rand_ipv4_addr();

            if(i4hdr_o.rand_orig_id)
                i4hdr_o.orig_id = (unsigned short)retrieve_rand_int(P_UINT16);

            if(i4hdr_o.rand_orig_s_port)
                i4hdr_o.orig_s_port = (unsigned short)retrieve_rand_int(P_UINT16);

            if(i4hdr_o.rand_orig_d_port)
                i4hdr_o.orig_d_port = (unsigned short)retrieve_rand_int(P_UINT16);

            if(i4hdr_o.orig_s_addr == NULL)
	        fatal_error("No original source IP address defined");

	    if((ihn_saddr = libnet_name2addr4(pkt_d, i4hdr_o.orig_s_addr, 1)) == -1)
	        fatal_error("Invalid original source IP address: %s", i4hdr_o.orig_s_addr);

	    if(i4hdr_o.orig_d_addr == NULL)
	        fatal_error("No original destination IP address defined");

	    if((ihn_daddr = libnet_name2addr4(pkt_d, i4hdr_o.orig_d_addr, 1)) == -1)
	        fatal_error("Invalid original destination IP address: %s", i4hdr_o.orig_d_addr);

            ih_payload[0] = (i4hdr_o.orig_s_port >> 8) & 0xff;
	    ih_payload[1] = i4hdr_o.orig_s_port & 0xff;
	    ih_payload[2] = (i4hdr_o.orig_d_port >> 8) & 0xff;
	    ih_payload[3] = i4hdr_o.orig_d_port & 0xff;
	    ih_payload[4] = 0;
	    ih_payload[5] = 32;
	    ih_payload[6] = 0;
	    ih_payload[7] = 0; 

            ih_payload_len = 8;

	    if(i4hdr_o.type == ICMP_UNREACH)
	    {
#ifdef DEBUG
                fprintf(stdout, "DEBUG: Building ICMP unreachable header\n");
#endif

                hdr_len = ICMPV4_UNREACH_H;

                if(libnet_build_icmpv4_unreach(
                    i4hdr_o.type,
                    i4hdr_o.code,
                    0,
	            orig_hlen,
	            i4hdr_o.orig_tos,	
                    i4hdr_o.orig_id,
                    0,
                    i4hdr_o.orig_ttl,
                    i4hdr_o.orig_p,
                    i4hdr_o.orig_sum,
                    ihn_saddr,
                    ihn_daddr,
                    ih_payload,
                    ih_payload_len,
                    pkt_d,
                    0) == -1)
                {
                    fatal_error("Unable to build ICMPv4 unreach header: %s", libnet_geterror(pkt_d));
	        }
            }
	    else if(i4hdr_o.type == ICMP_REDIRECT)
	    { 
#ifdef DEBUG
                fprintf(stdout, "DEBUG: Building ICMP redirect header\n");
#endif

		hdr_len = ICMPV4_REDIRECT_H;

                if(i4hdr_o.rand_gw)
	            i4hdr_o.gw = retrieve_rand_ipv4_addr();

                if(i4hdr_o.gw == NULL)
	            fatal_error("No gateway IP address defined");

                if((ihn_gw = libnet_name2addr4(pkt_d, i4hdr_o.gw, 1)) == -1)
	            fatal_error("Invalid gateway IP address: %s", i4hdr_o.gw);

 	        if(libnet_build_icmpv4_redirect(
	            i4hdr_o.type,
	            i4hdr_o.code,
	            0,
	            ihn_gw,
	            orig_hlen, 
	            i4hdr_o.orig_tos,
	            i4hdr_o.orig_id,
	            0,
	            i4hdr_o.orig_ttl,
	            i4hdr_o.orig_p,
	            i4hdr_o.orig_sum,
                    ihn_saddr,
	            ihn_daddr,
	            ih_payload,
	            ih_payload_len,
	            pkt_d,
	            0) == -1)
 	        {
	            fatal_error("Unable to build ICMPv4 redirect header: %s", libnet_geterror(pkt_d));
	        }
	    }
            else if(i4hdr_o.type == ICMP_TIMXCEED)
	    {
#ifdef DEBUG
                fprintf(stdout, "DEBUG: Building ICMP timelimit exceeded header\n");
#endif

                hdr_len = ICMPV4_TIMXCEED_H;

                if(libnet_build_icmpv4_timeexceed(
                    i4hdr_o.type,
	            i4hdr_o.code,
                    0,
	            orig_hlen,
	            i4hdr_o.orig_tos,
	            i4hdr_o.orig_id,
	            0,
                    i4hdr_o.orig_ttl,
	            i4hdr_o.orig_p, 
	            i4hdr_o.orig_sum,
	            ihn_saddr,
	            ihn_daddr,
	            ih_payload,
	            ih_payload_len,
	            pkt_d,
	            0) == -1)
	        {
	            fatal_error("Unable to build ICMPv4 timeexceed header: %s", libnet_geterror(pkt_d));
	        }
	    }

            break;

        case ICMP_TSTAMP: case ICMP_TSTAMPREPLY:
#ifdef DEBUG
            fprintf(stdout, "DEBUG: Building ICMP timestamp header\n");
#endif

            if(i4hdr_o.rand_seqn)
                i4hdr_o.seqn = (unsigned short)retrieve_rand_int(P_UINT16);

            if(i4hdr_o.rand_id)
                i4hdr_o.id = (unsigned short)retrieve_rand_int(P_UINT16);

            hdr_len = ICMPV4_TSTAMP_H;

            if(libnet_build_icmpv4_timestamp(
                i4hdr_o.type,
                i4hdr_o.code,
                0,
		i4hdr_o.id,
		i4hdr_o.seqn,
		i4hdr_o.otime,
		i4hdr_o.rtime,
		i4hdr_o.ttime,
		payload,
		payload_len,
		pkt_d,
		0) == -1)
	    {
		fatal_error("Unable to build ICMPv4 timestamp header: %s", libnet_geterror(pkt_d));
	    }
				   
            break;

        case ICMP_MASKREQ: case ICMP_MASKREPLY:
#ifdef DEBUG
            fprintf(stdout, "DEBUG: Building ICMP mask header\n");
#endif

            if(i4hdr_o.rand_seqn)
                i4hdr_o.seqn = (unsigned short)retrieve_rand_int(P_UINT16);

            if(i4hdr_o.rand_id)
                i4hdr_o.id = (unsigned short)retrieve_rand_int(P_UINT16);

            if(i4hdr_o.mask != NULL)
                if((ihn_mask = libnet_name2addr4(pkt_d, i4hdr_o.mask, 1)) == -1)
	            fatal_error("Invalid mask address: %s", i4hdr_o.mask);

            hdr_len = ICMPV4_MASK_H;

	    if(libnet_build_icmpv4_mask(
	        i4hdr_o.type,
	        i4hdr_o.code,
	        (i4hdr_o.mask != NULL) ? ihn_mask : 0,
	        i4hdr_o.id,
	        i4hdr_o.seqn,
		ihn_mask, 
	        payload,
	        payload_len,
	        pkt_d,
	        0) == -1)
	    {
	        fatal_error("Unable to build ICMPv4 mask header: %s", libnet_geterror(pkt_d));
	    }

            break;
    }

    return pkt_d; 
}

