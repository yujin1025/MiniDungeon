#include "pch.h"
#include "Object.h"

Object::Object()
{
	objectInfo = new Protocol::ObjectInfo();
	Protocol::PosInfo* posInfo = new Protocol::PosInfo();
	objectInfo->set_allocated_pos_info(posInfo);
}

Object::~Object()
{
	delete objectInfo;
	objectInfo = nullptr;
}
