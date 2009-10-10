/*
 * Generated by asn1c-0.9.22 (http://lionet.info/asn1c)
 * From ASN.1 module "POKERTH-PROTOCOL"
 * 	found in "../../../docs/pokerth.asn1"
 * 	`asn1c -fnative-types`
 */

#include <asn_internal.h>

#include "ChatRequestMessage.h"

static int
memb_chatText_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	const UTF8String_t *st = (const UTF8String_t *)sptr;
	size_t size;
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	size = UTF8String_length(st);
	if((ssize_t)size < 0) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: UTF-8: broken encoding (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	if((size >= 1 && size <= 128)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static asn_TYPE_member_t asn_MBR_chatRequestType_2[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct chatRequestType, choice.chatRequestTypeLobby),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_ChatRequestTypeLobby,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"chatRequestTypeLobby"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct chatRequestType, choice.chatRequestTypeGame),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_ChatRequestTypeGame,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"chatRequestTypeGame"
		},
};
static asn_TYPE_tag2member_t asn_MAP_chatRequestType_tag2el_2[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* chatRequestTypeLobby at 555 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 } /* chatRequestTypeGame at 557 */
};
static asn_CHOICE_specifics_t asn_SPC_chatRequestType_specs_2 = {
	sizeof(struct chatRequestType),
	offsetof(struct chatRequestType, _asn_ctx),
	offsetof(struct chatRequestType, present),
	sizeof(((struct chatRequestType *)0)->present),
	asn_MAP_chatRequestType_tag2el_2,
	2,	/* Count of tags in the map */
	0,
	2	/* Extensions start */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_chatRequestType_2 = {
	"chatRequestType",
	"chatRequestType",
	CHOICE_free,
	CHOICE_print,
	CHOICE_constraint,
	CHOICE_decode_ber,
	CHOICE_encode_der,
	CHOICE_decode_xer,
	CHOICE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	CHOICE_outmost_tag,
	0,	/* No effective tags (pointer) */
	0,	/* No effective tags (count) */
	0,	/* No tags (pointer) */
	0,	/* No tags (count) */
	0,	/* No PER visible constraints */
	asn_MBR_chatRequestType_2,
	2,	/* Elements count */
	&asn_SPC_chatRequestType_specs_2	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_ChatRequestMessage_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct ChatRequestMessage, chatRequestType),
		-1 /* Ambiguous tag (CHOICE?) */,
		0,
		&asn_DEF_chatRequestType_2,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"chatRequestType"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct ChatRequestMessage, chatText),
		(ASN_TAG_CLASS_UNIVERSAL | (12 << 2)),
		0,
		&asn_DEF_UTF8String,
		memb_chatText_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"chatText"
		},
};
static ber_tlv_tag_t asn_DEF_ChatRequestMessage_tags_1[] = {
	(ASN_TAG_CLASS_APPLICATION | (129 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_ChatRequestMessage_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (12 << 2)), 1, 0, 0 }, /* chatText at 558 */
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* chatRequestTypeLobby at 555 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 0, 0, 0 } /* chatRequestTypeGame at 557 */
};
static asn_SEQUENCE_specifics_t asn_SPC_ChatRequestMessage_specs_1 = {
	sizeof(struct ChatRequestMessage),
	offsetof(struct ChatRequestMessage, _asn_ctx),
	asn_MAP_ChatRequestMessage_tag2el_1,
	3,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	1,	/* Start extensions */
	3	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_ChatRequestMessage = {
	"ChatRequestMessage",
	"ChatRequestMessage",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_ChatRequestMessage_tags_1,
	sizeof(asn_DEF_ChatRequestMessage_tags_1)
		/sizeof(asn_DEF_ChatRequestMessage_tags_1[0]) - 1, /* 1 */
	asn_DEF_ChatRequestMessage_tags_1,	/* Same as above */
	sizeof(asn_DEF_ChatRequestMessage_tags_1)
		/sizeof(asn_DEF_ChatRequestMessage_tags_1[0]), /* 2 */
	0,	/* No PER visible constraints */
	asn_MBR_ChatRequestMessage_1,
	2,	/* Elements count */
	&asn_SPC_ChatRequestMessage_specs_1	/* Additional specs */
};

