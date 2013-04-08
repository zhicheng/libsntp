#ifndef __SNTP_H__
#define __SNTP_H__

#include <stdio.h>
#include <stdint.h>
#include <time.h>

/*
 * RFC 2030,Section 3
 *
 *                      1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                           Seconds                             |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                  Seconds Fraction (0-padded)                  |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */

struct timestamp {
	uint32_t seconds;
	uint32_t fraction;
};

struct shorttimestamp {
	uint16_t seconds;
	uint16_t fraction;
};

/*
 * RFC 2030,Section 4
 *
 *                         1                   2                   3
 *       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *      |LI | VN  |Mode |    Stratum    |     Poll      |   Precision   |
 *      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *      |                          Root Delay                           |
 *      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *      |                       Root Dispersion                         |
 *      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *      |                     Reference Identifier                      |
 *      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *      |                                                               |
 *      |                   Reference Timestamp (64)                    |
 *      |                                                               |
 *      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *      |                                                               |
 *      |                   Originate Timestamp (64)                    |
 *      |                                                               |
 *      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *      |                                                               |
 *      |                    Receive Timestamp (64)                     |
 *      |                                                               |
 *      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *      |                                                               |
 *      |                    Transmit Timestamp (64)                    |
 *      |                                                               |
 *      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *      |                 Key Identifier (optional) (32)                |
 *      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *      |                                                               |
 *      |                                                               |
 *      |                 Message Digest (optional) (128)               |
 *      |                                                               |
 *      |                                                               |
 *      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

struct sntp_packet {
	uint8_t status;
	uint8_t stratum;
	uint8_t poll;
	uint8_t precision;
	struct shorttimestamp rootdelay;
	struct shorttimestamp rootdispersion;
	uint32_t refid;
	
	struct timestamp reftime;
	struct timestamp orgtime;
	struct timestamp rectime;
	struct timestamp xmttime;
};

/* Read LI,VN and MODE from sntp.status 	*/
/* example:					*/
/* (sntp.status & LI_MASK) == LI_NOWARNING 	*/

#define LI_MASK		0xC0		/* 0b11000000  */
#define LI_NOWARNING    (0 << 6)        /* No warning.			*/
#define LI_PLUSSEC      (1 << 6)        /* Last minute has 61 seconds. 	*/
#define LI_MINUSSEC     (2 << 6)        /* Last minute has 59 seconds. 	*/
#define LI_ALARM        (3 << 6)        /* Alarm condition (clock not synchronized). */

#define VN_MASK		0x38		/* 0b00111000 */
#define VN_V1		(1 << 3)
#define VN_V2		(2 << 3)
#define VN_V3		(3 << 3)
#define VN_V4		(4 << 3)

#define MODE_MASK	0x7		/* 0b00000111 */
#define MODE_RSVD0	0		/* Reserved. 				*/
#define MODE_SYM_ACT	1		/* Symmetric active. 			*/
#define MODE_SYM_PAS	2		/* Symmetric passive. 			*/
#define MODE_CLIENT	3		/* Client. 				*/
#define MODE_SERVER	4		/* Server. 				*/
#define MODE_BROADCAST	5		/* Broadcast. 				*/
#define MODE_RSVD1	6		/* Reserved for NTP control message 	*/
#define MODE_RSVD2	6		/* Reserved for private use. 		*/

#define JAN_1970	2208988800UL	/* 1970 - 1900 in seconds */

void
sntp_sendto(int sock, struct sockaddr_in *addr);

time_t
sntp_recvfrom(int sock, struct sockaddr_in *addr, time_t orgtime);


#endif /* __SNTP_H__ */

