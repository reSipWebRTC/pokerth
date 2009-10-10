/*
 * Generated by asn1c-0.9.22 (http://lionet.info/asn1c)
 * From ASN.1 module "CHATCLEANER-PROTOCOL"
 * 	found in "../../../docs/chatcleaner.asn1"
 * 	`asn1c -fnative-types`
 */

#include <asn_internal.h>

#include "CleanerInitAckMessage.h"

static int
memb_serverVersion_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	long value;
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= 0 && value <= 65535)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_serverSecret_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
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
	
	if((size >= 1 && size <= 32)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static asn_TYPE_member_t asn_MBR_CleanerInitAckMessage_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct CleanerInitAckMessage, serverVersion),
		(ASN_TAG_CLASS_UNIVERSAL | (2 << 2)),
		0,
		&asn_DEF_NativeInteger,
		memb_serverVersion_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"serverVersion"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct CleanerInitAckMessage, serverSecret),
		(ASN_TAG_CLASS_UNIVERSAL | (12 << 2)),
		0,
		&asn_DEF_UTF8String,
		memb_serverSecret_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"serverSecret"
		},
};
static ber_tlv_tag_t asn_DEF_CleanerInitAckMessage_tags_1[] = {
	(ASN_TAG_CLASS_APPLICATION | (1 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_CleanerInitAckMessage_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (2 << 2)), 0, 0, 0 }, /* serverVersion at 39 */
    { (ASN_TAG_CLASS_UNIVERSAL | (12 << 2)), 1, 0, 0 } /* serverSecret at 40 */
};
static asn_SEQUENCE_specifics_t asn_SPC_CleanerInitAckMessage_specs_1 = {
	sizeof(struct CleanerInitAckMessage),
	offsetof(struct CleanerInitAckMessage, _asn_ctx),
	asn_MAP_CleanerInitAckMessage_tag2el_1,
	2,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	1,	/* Start extensions */
	3	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_CleanerInitAckMessage = {
	"CleanerInitAckMessage",
	"CleanerInitAckMessage",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_CleanerInitAckMessage_tags_1,
	sizeof(asn_DEF_CleanerInitAckMessage_tags_1)
		/sizeof(asn_DEF_CleanerInitAckMessage_tags_1[0]) - 1, /* 1 */
	asn_DEF_CleanerInitAckMessage_tags_1,	/* Same as above */
	sizeof(asn_DEF_CleanerInitAckMessage_tags_1)
		/sizeof(asn_DEF_CleanerInitAckMessage_tags_1[0]), /* 2 */
	0,	/* No PER visible constraints */
	asn_MBR_CleanerInitAckMessage_1,
	2,	/* Elements count */
	&asn_SPC_CleanerInitAckMessage_specs_1	/* Additional specs */
};

