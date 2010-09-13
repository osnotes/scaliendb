#include "SDBPControllerContext.h"
#include "Application/SDBP/SDBPConnection.h"

void SDBPControllerContext::SetController(Controller* controller_)
{
	controller = controller_;
}

bool SDBPControllerContext::IsValidRequest(ClientRequest& request)
{
	 return request.IsControllerRequest();
}

bool SDBPControllerContext::ProcessRequest(SDBPConnection* conn, ClientRequest& request)
{
	ConfigCommand	command;
	ClientResponse	response;
	
	if (request.type == CLIENTREQUEST_GET_MASTER)
	{
		if (!controller->IsMasterKnown())
			response.Failed(request.commandID);
		else
			response.Number(request.commandID, controller->GetMaster());
		conn->Write(response);
	}
	
	command = ConvertRequest(request);
	
	return controller->ProcessClientCommand(conn, command);
}

void SDBPControllerContext::OnComplete(SDBPConnection* conn, Command* command_)
{
	ConfigCommand* command = (ConfigCommand*) command_;
	
	// TODO:
	
	delete command;
}

ConfigCommand SDBPControllerContext::ConvertRequest(ClientRequest& request)
{
	ConfigCommand command;
	
	switch (request.type)
	{
		case CLIENTREQUEST_CREATE_QUORUM:
			command.type = CONFIG_CREATE_QUORUM;
			command.productionType = request.productionType;
			command.nodes = request.nodes;
			break;
		case CLIENTREQUEST_CREATE_DATABASE:
			command.type = CONFIG_CREATE_DATABASE;
			command.name = request.name;
			command.productionType = request.productionType;
			break;
		case CLIENTREQUEST_RENAME_DATABASE:
			command.type = CONFIG_RENAME_DATABASE;
			command.databaseID = request.databaseID;
			command.name = request.name;
			break;
		case CLIENTREQUEST_DELETE_DATABASE:
			command.type = CONFIG_DELETE_DATABASE;
			command.databaseID = request.databaseID;
			break;
		case CLIENTREQUEST_CREATE_TABLE:
			command.type = CONFIG_CREATE_TABLE;
			command.databaseID = request.databaseID;
			command.quorumID = request.quorumID;
			command.name = request.name;
			break;
		case CLIENTREQUEST_RENAME_TABLE:
			command.type = CONFIG_RENAME_TABLE;
			command.databaseID = request.databaseID;
			command.tableID = request.tableID;
			command.name = request.name;
			break;
		case CLIENTREQUEST_DELETE_TABLE:
			command.type = CONFIG_DELETE_TABLE;
			command.databaseID = request.databaseID;
			command.tableID = request.tableID;
			break;
	}
	
	return command;
}
