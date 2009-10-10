/*
 * Generated by asn1c-0.9.22 (http://lionet.info/asn1c)
 * From ASN.1 module "POKERTH-PROTOCOL"
 * 	found in "../../../docs/pokerth.asn1"
 * 	`asn1c -fnative-types`
 */

#ifndef	_RemovedFromGame_H_
#define	_RemovedFromGame_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeEnumerated.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum removedFromGameReason {
	removedFromGameReason_removedOnRequest	= 0,
	removedFromGameReason_kickedFromGame	= 1,
	removedFromGameReason_gameIsFull	= 2,
	removedFromGameReason_gameIsRunning	= 3,
	removedFromGameReason_gameTimeout	= 4,
	removedFromGameReason_removedStartFailed	= 5
} e_removedFromGameReason;

/* RemovedFromGame */
typedef struct RemovedFromGame {
	long	 removedFromGameReason;
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} RemovedFromGame_t;

/* Implementation */
/* extern asn_TYPE_descriptor_t asn_DEF_removedFromGameReason_2;	// (Use -fall-defs-global to expose) */
extern asn_TYPE_descriptor_t asn_DEF_RemovedFromGame;

#ifdef __cplusplus
}
#endif

#endif	/* _RemovedFromGame_H_ */
