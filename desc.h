/*
 * Copyright (c) 2010  Axel Neumann
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of version 2 of the GNU General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */

#include <stdint.h>
#include <sys/types.h>
#include <netinet/in.h>

#define ARG_DESC_ROOT_SIZE         "descRootSizeOut"
extern int32_t desc_root_size_out;
#define HLP_DESC_ROOT_SIZE         "set maximum size for own description and references"
#define MIN_DESC_ROOT_SIZE               (MIN_UDPD_SIZE - sizeof(struct packet_header) - (2*sizeof(struct tlv_hdr)))
#define MAX_DESC_ROOT_SIZE               (MAX_UDPD_SIZE - sizeof(struct packet_header) - (2*sizeof(struct tlv_hdr)))
#define DEF_DESC_ROOT_SIZE               MAX_DESC_ROOT_SIZE
#define     REF_CONTENT_BODY_SIZE_OUT (desc_root_size_out - sizeof(struct frame_hdr_content_adv))
#define     REF_CONTENT_BODY_SIZE_MAX (MAX_DESC_ROOT_SIZE - sizeof(struct frame_hdr_content_adv))

extern int32_t vrt_frame_max_nesting;


#define ARG_VRT_FRAME_DATA_SIZE_OUT  "descVirtFrameSizeOut"
#define HLP_VRT_FRAME_DATA_SIZE_OUT  "set maximum virtual size for own description frames"
#define ARG_VRT_FRAME_DATA_SIZE_IN   "descVirtFrameSizeIn"
#define HLP_VRT_FRAME_DATA_SIZE_IN   "set maximum virtual size for other description frames"
#define MIN_VRT_FRAME_DATA_SIZE      (MIN_DESC_ROOT_SIZE)
#define MAX_VRT_FRAME_DATA_SIZE      (4*((REF_CONTENT_BODY_SIZE_MAX / sizeof(SHA1_T)) * REF_CONTENT_BODY_SIZE_MAX))
#define DEF_VRT_FRAME_DATA_SIZE      (2*((REF_CONTENT_BODY_SIZE_MAX / sizeof(SHA1_T)) * REF_CONTENT_BODY_SIZE_MAX))
extern int32_t vrt_frame_data_size_in;
extern int32_t vrt_frame_data_size_out;



#define ARG_DESC_VBODIES_SIZE_OUT  "descVirtSizeOut"
#define HLP_DESC_VBODIES_SIZE_OUT  "set maximum virtual size for own description"
#define ARG_DESC_VBODIES_SIZE_IN   "descVirtSizeIn"
#define HLP_DESC_VBODIES_SIZE_IN   "set maximum virtual size for other node descriptions"
#define MIN_DESC_VBODIES_SIZE      (MIN_DESC_ROOT_SIZE)
// this should be the max possible with a reference depth of 1 :
#define MAX_DESC_VBODIES_SIZE      (10 * MAX_VRT_FRAME_DATA_SIZE)
#define DEF_DESC_VBODIES_SIZE      ( 4 * MAX_VRT_FRAME_DATA_SIZE)

extern int32_t desc_vbodies_size_in;
extern int32_t desc_vbodies_size_out;

#define MIN_DESC0_REFERRED_TO 10000
#define MAX_DESC0_REFERRED_TO 100000
#define DEF_DESC0_REFERRED_TO 10000

#define DEF_UNSOLICITED_DESC_ADVS 1
#define MIN_UNSOLICITED_DESC_ADVS 0
#define MAX_UNSOLICITED_DESC_ADVS 1
#define ARG_UNSOLICITED_DESC_ADVS "unsolicitedDescAdvs"

#define ARG_DSQN_PATH "descSqnPath"
#define DEF_DSQN_PATH "/etc/bmx7/descSqn"


#define ARG_DESCRIPTIONS        "descriptions"
#define HLP_DESCRIPTIONS        "show node descriptions\n"

#define ARG_DESCRIPTION_NAME    "name"

#define ARG_DESCRIPTION_TYPE    "type"
#define DEF_DESCRIPTION_TYPE     FRAME_TYPE_PROCESS_ALL
#define MIN_DESCRIPTION_TYPE     0
#define MAX_DESCRIPTION_TYPE     FRAME_TYPE_PROCESS_ALL
#define HLP_DESCRIPTION_TYPE     "show description extension(s) of given type (0..253=type 254=none 255=all) \n"

#define MIN_REF_MAINTAIN_INTERVAL 1
#define MAX_REF_MAINTAIN_INTERVAL 1000000
#define DEF_REF_MAINTAIN_INTERVAL 5000
#define ARG_REF_MAINTAIN_INTERVAL "maintainanceInterval"
extern int32_t maintainanceInterval;

#define MIN_DHASH_RSLV_ITERS 1
#define MAX_DHASH_RSLV_ITERS 20
#define DEF_DHASH_RSLV_ITERS 4
#define ARG_DHASH_RSLV_ITERS "resolveIterations"
extern int32_t resolveIterations;

#define MIN_DHASH_RSLV_INTERVAL 100
#define MAX_DHASH_RSLV_INTERVAL 10000
#define DEF_DHASH_RSLV_INTERVAL 200
#define ARG_DHASH_RSLV_INTERVAL "resolveInterval"
extern int32_t resolveInterval;

struct msg_iid_adv {
	IID_T transmitterIID4x;
	DESC_SQN_T descSqn;
	ChainLink_T chainOgm;
	CRYPTSHA1_T nodeId;
} __attribute__((packed));

struct msg_iid_request {
	IID_T receiverIID4x;
} __attribute__((packed));

struct hdr_iid_request {
	GLOBAL_ID_T dest_nodeId;
	struct msg_iid_request msg[];
} __attribute__((packed));

struct schedule_dsc_req {
	IID_T iid;
	DESC_SQN_T descSqn;
};

struct msg_description_request {
	CRYPTSHA1_T kHash;
} __attribute__((packed));

struct hdr_description_request {
	DHASH_T dest_kHash;
	struct msg_description_request msg[];
} __attribute__((packed));








struct description_msg_name {
	uint8_t type;
	uint8_t len;
	char name[];
} __attribute__((packed));

#define DESCRIPTION_MSG_NAME_FORMAT { \
{FIELD_TYPE_UINT,          -1, 8, 0, FIELD_RELEVANCE_HIGH, "type"}, \
{FIELD_TYPE_STRING_SIZE,   -1, 8, 0, FIELD_RELEVANCE_LOW,  "len"},  \
{FIELD_TYPE_STRING_BINARY, -1, 0, 0, FIELD_RELEVANCE_LOW,  "name" },  \
FIELD_FORMAT_END }




void process_description_tlvs_del(struct orig_node *on, struct desc_content *dcOld, uint8_t ft_start, uint8_t ft_end);
IDM_T process_description_tlvs(struct packet_buff *pb, struct orig_node *on, struct desc_content *dcOld, struct desc_content *dcOp, uint8_t op, uint8_t filter);
void update_my_description(void);

void update_orig_dhash(struct desc_content *dc);

SHA1_T *nodeIdFromDescAdv(uint8_t *desc_adv);
char *nodeIdAsStringFromDescAdv(uint8_t *desc_adv);
IDM_T desc_frame_changed(struct desc_content *dcA, struct desc_content *dcB, uint8_t type);

int32_t opt_update_dext_method(uint8_t cmd, uint8_t _save, struct opt_type *opt, struct opt_parent *patch, struct ctrl_node *cn);
int32_t opt_update_description(uint8_t cmd, uint8_t _save, struct opt_type *opt, struct opt_parent *patch, struct ctrl_node *cn);

void init_desc(void);


