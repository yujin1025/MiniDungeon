#pragma once


class Object : public enable_shared_from_this<Object>
{
public:
	Object();
	virtual ~Object();

	bool IsPlayer() { return _isPlayer; }

public:
	Protocol::ObjectInfo* GetObjectInfo() { return objectInfo; }
	void SetObjectInfo(Protocol::ObjectInfo* objectInfo) { this->objectInfo = objectInfo; }

	Protocol::PosInfo* GetPosInfo() { return posInfo; }
	void SetPosInfo(Protocol::PosInfo* posInfo) { this->posInfo = posInfo; }

public:
	atomic<weak_ptr<Room>> room;

protected:
	bool _isPlayer = false;
	Protocol::ObjectInfo* objectInfo;
	Protocol::PosInfo* posInfo;
};

