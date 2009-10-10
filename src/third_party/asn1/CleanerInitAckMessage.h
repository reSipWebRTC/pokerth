/*
 * Generated by asn1c-0.9.22 (http://lionet.info/asn1c)
 * From ASN.1 module "CHATCLEANER-PROTOCOL"
 * 	found in "../../../docs/chatcleaner.asn1"
 * 	`asn1c -fnative-types`
 */

#ifndef	_CleanerInitAckMessage_H_
#define	_CleanerInitAckMessage_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <UTF8String.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* CleanerInitAckMessage */
typedef struct CleanerInitAckMessage {
	long	 serverVersion;
	UTF8String_t	 serverSecret;
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} CleanerInitAckMessage_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CleanerInitAckMessage;

#ifdef __cplusplus
}
#endif

#endif	/* _CleanerInitAckMessage_H_ */
