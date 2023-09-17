#include "stdafx.h"
#include "MessageHandler.h"

MessageHandler::DebugMessages MessageHandler::debugMessages;


void MessageHandler::DebugMessages::AddMessage(string msg)
{
	if (!EDITORMODE) return;
	if (myCollapseToggle)
	{
		if (myMessagesAmmount.find(msg) == myMessagesAmmount.end())
		{
			myMessagesAmmount.insert({ msg, 1 });
			messages.push_back(msg);
		}
		else
		{
			myMessagesAmmount[msg]++;
		}
	}
	else
	{
		messages.push_back(msg);
	}
}

void MessageHandler::DebugMessages::AddError(string msg)
{
	if (!EDITORMODE) return;
	if (myCollapseToggle)
	{
		if (myErrosAmmount.find(msg) == myErrosAmmount.end())
		{
			myErrosAmmount.insert({ msg, 1 });
			errors.push_back(msg);
		}
		else
		{
			myErrosAmmount[msg]++;
		}
	}
	else
	{
		errors.push_back(msg);
	}
}