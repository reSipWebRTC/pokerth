/*
 * Generated by asn1c-0.9.22 (http://lionet.info/asn1c)
 * From ASN.1 module "POKERTH-PROTOCOL"
 * 	found in "../../../docs/pokerth.asn1"
 * 	`asn1c -fnative-types`
 */

#ifndef	_KickPlayerRequestMessage_H_
#define	_KickPlayerRequestMessage_H_


#include <asn_application.h>

/* Including external dependencies */
#include "NonZeroId.h"
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* KickPlayerRequestMessage */
typedef struct KickPlayerRequestMessage {
	NonZeroId_t	 gameId;
	NonZeroId_t	 playerId;
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} KickPlayerRequestMessage_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_KickPlayerRequestMessage;

#ifdef __cplusplus
}
#endif

#endif	/* _KickPlayerRequestMessage_H_ */
