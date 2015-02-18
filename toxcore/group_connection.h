/* group_connection.h
 *
 * An implementation of massive text only group chats.
 *
 *
 *  Copyright (C) 2015 Tox project All Rights Reserved.
 *
 *  This file is part of Tox.
 *
 *  Tox is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Tox is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Tox.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef GROUP_CONNECTION_H
#define GROUP_CONNECTION_H

#include "group_chats.h"

#define GCC_BUFFER_SIZE 20000    /* must fit inside an uint_16 */

struct GC_Message_Ary {
    uint8_t *data;
    uint32_t data_length;
    uint8_t  packet_type;
    uint64_t message_id;
    uint64_t time_added;
    uint64_t last_send_try;
};

typedef struct GC_Connection {
    uint64_t send_message_id;
    uint16_t send_ary_start;   /* send_ary index of oldest item */
    uint64_t last_send_try;
    struct GC_Message_Ary send_ary[GCC_BUFFER_SIZE];

    uint64_t recv_message_id;
    struct GC_Message_Ary recv_ary[GCC_BUFFER_SIZE];
} GC_Connection;


/* Adds data of length to peernum's send_ary.
 *
 * Returns 0 on success and increments peernum's send_message_id.
 * Returns -1 on failure.
 */
int gcc_add_send_ary(GC_Chat *chat, const uint8_t *data, uint32_t length, uint32_t peernum,
                     uint8_t packet_type);

/* Decides if message need to be put in recv_ary or immediately handled.
 *
 * Return 1 if message is in correct sequence and may be handled immediately.
 * Return 0 if message is a duplicate or put in ary.
 * Return -1 on failure
 */
int gcc_handle_recv_message(GC_Chat *chat, uint32_t peernum, const uint8_t *data, uint32_t length,
                            uint8_t packet_type, uint64_t message_id);

/* Returns ary index for message_id */
uint16_t get_ary_index(const struct GC_Message_Ary *ary, uint64_t message_id);

/* Removes send_ary item with message_id.
 *
 * Returns 0 if success.
 * Returns -1 on failure.
 */
int gcc_handle_ack(GC_Connection *gconn, uint64_t message_id);

/* Checks for and handles messages that are in proper sequence in peernum's recv_ary.
 * This should always be called after a new packet is successfully handled.
 *
 * Return 0 on success.
 * Return -1 on failure.
 */
int gcc_check_recv_ary(Messenger *m, int groupnum, int peernum);

void gcc_resend_packets(Messenger *m, GC_Chat *chat, uint32_t peernumber);

/* called when a peer leaves the group */
void gcc_peer_cleanup(GC_Connection *gconn);

/* called on group exit */
void gcc_cleanup(GC_Chat *chat);

#endif  /* GROUP_CONNECTION_H */