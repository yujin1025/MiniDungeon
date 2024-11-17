#pragma once


class Object : public enable_shared_from_this<Object>
{
public:
	Object();
	virtual ~Object();

	bool IsPlayer() { return _isPlayer; }

public:
	const Protocol::ObjectInfo* GetObjectInfo() { return objectInfo; }

	const Protocol::PosInfo GetPosInfo() { return objectInfo->pos_info(); }
public:
	atomic<weak_ptr<Room>> room;

protected:
	bool _isPlayer = false;
	
protected:
	Protocol::ObjectInfo* objectInfo;
};

