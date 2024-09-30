#pragma once
#include "DBConnection.h"

template<int32 C>
struct FullBits { enum { value = (1 << (C - 1)) | FullBits<C - 1>::value }; };

template<>
struct FullBits<1> { enum { value = 1 }; };

template<>
struct FullBits<0> { enum { value = 0 }; };

template<int32 ParamCount, int32 ColumnCount>
class DBBind
{
public:
	DBBind(DBConnection& dbConnection, const WCHAR* query)
		: _dbConnection(dbConnection), _query(query)
	{
		::memset(_paramIndex, 0, sizeof(_paramIndex));
		::memset(_columnIndex, 0, sizeof(_columnIndex));
		_paramFlag = 0;
		_columnFlag = 0;
		_dbConnection.Unbind();
	}

	bool Validate()
	{
		return _paramFlag == FullBits<ParamCount>::value && _columnFlag == FullBits<ColumnCount>::value;
	}

	bool Execute()
	{
		ASSERT_CRASH(Validate());
		return _dbConnection.Execute(_query);
	}

	bool Fetch()
	{
		return _dbConnection.Fetch();
	}

public:
	template<typename T>
	void BindParam(int32 index, T* value)
	{
		_dbConnection.BindParam(index + 1, &value, &_paramIndex[index]);
		_paramFlag |= (1LL << index);
	}

	void BindParam(int32 index, const WCHAR* value)
	{
		_dbConnection.BindParam(index + 1, value, &_paramIndex[index]);
		_paramFlag |= (1LL << index);
	}

	void BindParam(int32 index, const std::wstring& value)
	{
		// std::wstring���� const WCHAR*�� ��ȯ�Ͽ� ����
		_dbConnection.BindParam(index + 1, value.c_str(), &_paramIndex[index]);
		_paramFlag |= (1LL << index);
	}

	template<typename T, int32 N>
	void BindParam(int32 index, T(&value)[N])
	{
		_dbConnection.BindParam(index + 1, (const BYTE*)value, size32(T) * N, &_paramIndex[index]);
		_paramFlag |= (1LL << index);
	}

	template<typename T>
	void BindParam(int32 index, T* value, int32 N)
	{
		_dbConnection.BindParam(index + 1, (const BYTE*)value, size32(T) * N, &_columnIndex[index]);
		_paramFlag |= (1LL << index);
	}

	template<typename T>
	void BindColumn(int32 index, T& value)
	{
		_dbConnection.BindColumn(index + 1, &value, &_columnIndex[index]);
		_columnFlag |= (1LL << index);
	}

	template<int32 N>
	void BindColumn(int32 index, WCHAR(&value)[N])
	{
		_dbConnection.BindColumn(index + 1, value, N - 1, &_columnIndex[index]);
		_columnFlag |= (1LL << index);
	}

	void BindColumn(int32 index, WCHAR* value, int32 len)
	{
		_dbConnection.BindColumn(index + 1, value, len - 1, &_columnIndex[index]);
		_columnFlag |= (1LL << index);
	}

	template<typename T, int32 N>
	void BindColumn(int32 index, T(&value)[N])
	{
		_dbConnection.BindColumn(index + 1, value, size32(T) * N, &_columnIndex[index]);
		_columnFlag |= (1LL << index);
	}
protected:
	DBConnection& _dbConnection;
	const WCHAR* _query;
	SQLLEN _paramIndex[ParamCount > 0 ? ParamCount : 1];
	SQLLEN _columnIndex[ColumnCount > 0 ? ColumnCount : 1];
	uint64 _paramFlag;
	uint64 _columnFlag;
};

