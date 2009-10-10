/*
 * Generated by asn1c-0.9.22 (http://lionet.info/asn1c)
 * From ASN.1 module "POKERTH-PROTOCOL"
 * 	found in "../../../docs/pokerth.asn1"
 * 	`asn1c -fnative-types`
 */

#ifndef	_StatisticsMessage_H_
#define	_StatisticsMessage_H_


#include <asn_application.h>

/* Including external dependencies */
#include <asn_SEQUENCE_OF.h>
#include <constr_SEQUENCE_OF.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct StatisticsData;

/* StatisticsMessage */
typedef struct StatisticsMessage {
	struct statisticsData {
		A_SEQUENCE_OF(struct StatisticsData) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} statisticsData;
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} StatisticsMessage_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_StatisticsMessage;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "StatisticsData.h"

#endif	/* _StatisticsMessage_H_ */
