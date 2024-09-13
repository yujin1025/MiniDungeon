#pragma once


class Object : public enable_shared_from_this<Object>
{
public:
	Object();
	virtual ~Object();

	bool IsPlayer() { return _isPlayer; }

public:
	const Protocol::ObjectInfo* GetObjectInfo() { return objectInfo; }
	void SetObjectInfo(Protocol::ObjectInfo* objectInfo) { this->objectInfo = objectInfo; }

	const Protocol::PosInfo* GetPosInfo() { return posInfo; }
	void SetPosInfo(Protocol::PosInfo* posInfo) { this->posInfo = posInfo; }

	void SetObjectID(int64 object_id) { this->objectInfo->set_object_id(object_id); this->posInfo->set_object_id(object_id); }
public:
	atomic<weak_ptr<Room>> room;

protected:
	bool _isPlayer = false;
	Protocol::ObjectInfo* objectInfo;
	Protocol::PosInfo* posInfo;
};

