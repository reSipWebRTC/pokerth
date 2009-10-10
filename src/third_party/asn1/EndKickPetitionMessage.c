/*
 * Generated by asn1c-0.9.22 (http://lionet.info/asn1c)
 * From ASN.1 module "POKERTH-PROTOCOL"
 * 	found in "../../../docs/pokerth.asn1"
 * 	`asn1c -fnative-types`
 */

#include <asn_internal.h>

#include "EndKickPetitionMessage.h"

static int
petitionEndReason_7_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	/* Replace with underlying type checker */
	td->check_constraints = asn_DEF_NativeEnumerated.check_constraints;
	return td->check_constraints(td, sptr, ctfailcb, app_key);
}

/*
 * This type is implemented using NativeEnumerated,
 * so here we adjust the DEF accordingly.
 */
static void
petitionEndReason_7_inherit_TYPE_descriptor(asn_TYPE_descriptor_t *td) {
	td->free_struct    = asn_DEF_NativeEnumerated.free_struct;
	td->print_struct   = asn_DEF_NativeEnumerated.print_struct;
	td->ber_decoder    = asn_DEF_NativeEnumerated.ber_decoder;
	td->der_encoder    = asn_DEF_NativeEnumerated.der_encoder;
	td->xer_decoder    = asn_DEF_NativeEnumerated.xer_decoder;
	td->xer_encoder    = asn_DEF_NativeEnumerated.xer_encoder;
	td->uper_decoder   = asn_DEF_NativeEnumerated.uper_decoder;
	td->uper_encoder   = asn_DEF_NativeEnumerated.uper_encoder;
	if(!td->per_constraints)
		td->per_constraints = asn_DEF_NativeEnumerated.per_constraints;
	td->elements       = asn_DEF_NativeEnumerated.elements;
	td->elements_count = asn_DEF_NativeEnumerated.elements_count;
     /* td->specifics      = asn_DEF_NativeEnumerated.specifics;	// Defined explicitly */
}

static void
petitionEndReason_7_free(asn_TYPE_descriptor_t *td,
		void *struct_ptr, int contents_only) {
	petitionEndReason_7_inherit_TYPE_descriptor(td);
	td->free_struct(td, struct_ptr, contents_only);
}

static int
petitionEndReason_7_print(asn_TYPE_descriptor_t *td, const void *struct_ptr,
		int ilevel, asn_app_consume_bytes_f *cb, void *app_key) {
	petitionEndReason_7_inherit_TYPE_descriptor(td);
	return td->print_struct(td, struct_ptr, ilevel, cb, app_key);
}

static asn_dec_rval_t
petitionEndReason_7_decode_ber(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const void *bufptr, size_t size, int tag_mode) {
	petitionEndReason_7_inherit_TYPE_descriptor(td);
	return td->ber_decoder(opt_codec_ctx, td, structure, bufptr, size, tag_mode);
}

static asn_enc_rval_t
petitionEndReason_7_encode_der(asn_TYPE_descriptor_t *td,
		void *structure, int tag_mode, ber_tlv_tag_t tag,
		asn_app_consume_bytes_f *cb, void *app_key) {
	petitionEndReason_7_inherit_TYPE_descriptor(td);
	return td->der_encoder(td, structure, tag_mode, tag, cb, app_key);
}

static asn_dec_rval_t
petitionEndReason_7_decode_xer(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const char *opt_mname, const void *bufptr, size_t size) {
	petitionEndReason_7_inherit_TYPE_descriptor(td);
	return td->xer_decoder(opt_codec_ctx, td, structure, opt_mname, bufptr, size);
}

static asn_enc_rval_t
petitionEndReason_7_encode_xer(asn_TYPE_descriptor_t *td, void *structure,
		int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	petitionEndReason_7_inherit_TYPE_descriptor(td);
	return td->xer_encoder(td, structure, ilevel, flags, cb, app_key);
}

static int
memb_numVotesAgainstKicking_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	long value;
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= 0 && value <= 9)) {
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
memb_numVotesInFavourOfKicking_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	long value;
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= 1 && value <= 9)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static asn_INTEGER_enum_map_t asn_MAP_petitionEndReason_value2enum_7[] = {
	{ 0,	22,	"petitionEndEnoughVotes" },
	{ 1,	24,	"petitionEndTooFewPlayers" },
	{ 2,	21,	"petitionEndPlayerLeft" },
	{ 3,	18,	"petitionEndTimeout" }
};
static unsigned int asn_MAP_petitionEndReason_enum2value_7[] = {
	0,	/* petitionEndEnoughVotes(0) */
	2,	/* petitionEndPlayerLeft(2) */
	3,	/* petitionEndTimeout(3) */
	1	/* petitionEndTooFewPlayers(1) */
};
static asn_INTEGER_specifics_t asn_SPC_petitionEndReason_specs_7 = {
	asn_MAP_petitionEndReason_value2enum_7,	/* "tag" => N; sorted by tag */
	asn_MAP_petitionEndReason_enum2value_7,	/* N => "tag"; sorted by N */
	4,	/* Number of elements in the maps */
	0,	/* Enumeration is not extensible */
	1,	/* Strict enumeration */
	0,	/* Native long size */
	0
};
static ber_tlv_tag_t asn_DEF_petitionEndReason_tags_7[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (10 << 2))
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_petitionEndReason_7 = {
	"petitionEndReason",
	"petitionEndReason",
	petitionEndReason_7_free,
	petitionEndReason_7_print,
	petitionEndReason_7_constraint,
	petitionEndReason_7_decode_ber,
	petitionEndReason_7_encode_der,
	petitionEndReason_7_decode_xer,
	petitionEndReason_7_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_petitionEndReason_tags_7,
	sizeof(asn_DEF_petitionEndReason_tags_7)
		/sizeof(asn_DEF_petitionEndReason_tags_7[0]), /* 1 */
	asn_DEF_petitionEndReason_tags_7,	/* Same as above */
	sizeof(asn_DEF_petitionEndReason_tags_7)
		/sizeof(asn_DEF_petitionEndReason_tags_7[0]), /* 1 */
	0,	/* No PER visible constraints */
	0, 0,	/* Defined elsewhere */
	&asn_SPC_petitionEndReason_specs_7	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_EndKickPetitionMessage_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct EndKickPetitionMessage, gameId),
		(ASN_TAG_CLASS_UNIVERSAL | (2 << 2)),
		0,
		&asn_DEF_NonZeroId,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"gameId"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct EndKickPetitionMessage, petitionId),
		(ASN_TAG_CLASS_UNIVERSAL | (2 << 2)),
		0,
		&asn_DEF_NonZeroId,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"petitionId"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct EndKickPetitionMessage, numVotesAgainstKicking),
		(ASN_TAG_CLASS_UNIVERSAL | (2 << 2)),
		0,
		&asn_DEF_NativeInteger,
		memb_numVotesAgainstKicking_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"numVotesAgainstKicking"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct EndKickPetitionMessage, numVotesInFavourOfKicking),
		(ASN_TAG_CLASS_UNIVERSAL | (2 << 2)),
		0,
		&asn_DEF_NativeInteger,
		memb_numVotesInFavourOfKicking_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"numVotesInFavourOfKicking"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct EndKickPetitionMessage, resultPlayerKicked),
		(ASN_TAG_CLASS_UNIVERSAL | (1 << 2)),
		0,
		&asn_DEF_BOOLEAN,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"resultPlayerKicked"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct EndKickPetitionMessage, petitionEndReason),
		(ASN_TAG_CLASS_UNIVERSAL | (10 << 2)),
		0,
		&asn_DEF_petitionEndReason_7,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"petitionEndReason"
		},
};
static ber_tlv_tag_t asn_DEF_EndKickPetitionMessage_tags_1[] = {
	(ASN_TAG_CLASS_APPLICATION | (70 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_EndKickPetitionMessage_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (1 << 2)), 4, 0, 0 }, /* resultPlayerKicked at 533 */
    { (ASN_TAG_CLASS_UNIVERSAL | (2 << 2)), 0, 0, 3 }, /* gameId at 529 */
    { (ASN_TAG_CLASS_UNIVERSAL | (2 << 2)), 1, -1, 2 }, /* petitionId at 530 */
    { (ASN_TAG_CLASS_UNIVERSAL | (2 << 2)), 2, -2, 1 }, /* numVotesAgainstKicking at 531 */
    { (ASN_TAG_CLASS_UNIVERSAL | (2 << 2)), 3, -3, 0 }, /* numVotesInFavourOfKicking at 532 */
    { (ASN_TAG_CLASS_UNIVERSAL | (10 << 2)), 5, 0, 0 } /* petitionEndReason at 535 */
};
static asn_SEQUENCE_specifics_t asn_SPC_EndKickPetitionMessage_specs_1 = {
	sizeof(struct EndKickPetitionMessage),
	offsetof(struct EndKickPetitionMessage, _asn_ctx),
	asn_MAP_EndKickPetitionMessage_tag2el_1,
	6,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	5,	/* Start extensions */
	7	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_EndKickPetitionMessage = {
	"EndKickPetitionMessage",
	"EndKickPetitionMessage",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_EndKickPetitionMessage_tags_1,
	sizeof(asn_DEF_EndKickPetitionMessage_tags_1)
		/sizeof(asn_DEF_EndKickPetitionMessage_tags_1[0]) - 1, /* 1 */
	asn_DEF_EndKickPetitionMessage_tags_1,	/* Same as above */
	sizeof(asn_DEF_EndKickPetitionMessage_tags_1)
		/sizeof(asn_DEF_EndKickPetitionMessage_tags_1[0]), /* 2 */
	0,	/* No PER visible constraints */
	asn_MBR_EndKickPetitionMessage_1,
	6,	/* Elements count */
	&asn_SPC_EndKickPetitionMessage_specs_1	/* Additional specs */
};

