#ifndef CLIENTRESPONSE_H
#define CLIENTRESPONSE_H

#include "System/Buffers/ReadBuffer.h"
#include "Application/Controller/State/ConfigState.h"

#define CLIENTRESPONSE_OK				'O'
#define CLIENTRESPONSE_NUMBER			'n'
#define CLIENTRESPONSE_VALUE			'V'
#define CLIENTRESPONSE_GET_CONFIG_STATE	'C'
#define CLIENTRESPONSE_NOTMASTER		'N'
#define CLIENTRESPONSE_FAILED			'F'

class ClientRequest; // forward

/*
===============================================================================================

 ClientResponse

===============================================================================================
*/

class ClientResponse
{
public:
	ClientRequest*	request;
	
	/* Variables */
	char			type;
	uint64_t		number;
	ReadBuffer		value;
	ConfigState*	configState;
	
	ClientResponse();
	~ClientResponse();
	
	ConfigState*	TransferConfigState();
		
	/* Responses */
	bool			OK();
	bool			Number(uint64_t number);
	bool			Value(ReadBuffer& value);
	bool			GetConfigStateResponse(ConfigState* configState);
	bool			NotMaster();
	bool			Failed();
};

#endif
