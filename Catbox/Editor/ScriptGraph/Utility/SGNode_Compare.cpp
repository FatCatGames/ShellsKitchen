#include "stdafx.h"
#include "SGNode_Compare.h"

void SGNode_Compare::Init()
{
	SetTitle("Compare");
	SetCategory("Utility");

	CreateExecPin("In", PinDirection::Input, true);
	CreateExecPin("Out", PinDirection::Output, true);
	CreateDataPin<void*>("Input1", PinDirection::Input);
	CreateDataPin<void*>("Input2", PinDirection::Input);
	CreateDataPin<bool>("Equal", PinDirection::Output);
}

size_t SGNode_Compare::DoOperation()
{
	void* data1 = nullptr;
	void* data2 = nullptr;
	GetPinData("Input1", data1);
	GetPinData("Input2", data2);
	SetPinData("Equal", data1 == data2);
	return ExitViaPin("Out");
}

void SGNode_Compare_String::Init()
{
	SetTitle("Compare String");
	SetCategory("Utility");

	CreateExecPin("In", PinDirection::Input, true);
	CreateExecPin("Out", PinDirection::Output, true);

	CreateDataPin<std::string>("Input1", PinDirection::Input);
	CreateDataPin<std::string>("Input2", PinDirection::Input);
	CreateDataPin<bool>("Equal", PinDirection::Output);
}

size_t SGNode_Compare_String::DoOperation()
{
	std::string data1;
	std::string data2;
	GetPinData("Input1", data1);
	GetPinData("Input2", data2);
	SetPinData("Equal", data1 == data2);
	return ExitViaPin("Out");
}
