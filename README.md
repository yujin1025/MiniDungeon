# 졸업프로젝트 👥

# MINIDUNGEON
![스크린샷 2024-11-09 212537](https://github.com/user-attachments/assets/8ecfc15b-7776-49a6-9c1a-eac265ee8493)


## 개발 기간 📅
#### 2024.03.01~ 2024.11.26

## 역할 분담 🧑‍💻
### 개발 인원 : 2명 
| 이름 | 개인 역할 | 담당 역할 및 기능 |
| ------ | ---------- | ------ |
| 한효빈 | Developer | 서버 |
| 강유진 | Developer | 클라이언트 |

## 시연영상 
#### ⬇ Link Here ⬇
https://youtu.be/ZupDwxTNN_M

## 구현 내용
### C++로 BehaviorTree를 구현하여 서버에서 몬스터 AI 실행
```{cpp}
    // 전방 선언
class Node;
class Blackboard;

// enum ENodeState는 행동 트리의 노드 상태를 나타냅니다.
enum class ENodeState {
    Running,   // 실행 중
    Success,   // 성공
    Failure    // 실패
};

// enum EBlackboardKey는 Blackboard에서 다양한 키를 액세스하기 위해 사용됩니다.
enum class EBlackboardKey {
    Target,
    Position
};

// 3D 벡터 구조체로, 위치를 저장하는 데 사용됩니다 (예시로 정의).
struct Vector3 {
    float x, y, z;
};

// BehaviourTree 클래스는 행동 트리의 루트 컨트롤러를 나타냅니다.
class BehaviourTree : public enable_shared_from_this<BehaviourTree>
{
public:
    BehaviourTree();
    virtual ~BehaviourTree();

    // 행동 트리를 업데이트하며, 루트 노드에서 시작합니다.
    ENodeState Update()
    {
        // 루트 노드가 실행 중일 경우 업데이트 수행
        if (rootNode && rootNode->state == ENodeState::Running)
        {
            treeState = rootNode->Update(); // 루트 노드를 업데이트하고 트리 상태를 저장
        }

        return treeState; // 트리의 현재 상태 반환
    }

    // 주어진 부모 노드의 자식 노드들을 반환하는 정적 함수
    static vector<shared_ptr<Node>> GetChildren(const std::shared_ptr<Node>& parent);

    // 트리를 순회하며 각 노드를 방문하는 정적 함수
    static void Traverse(const std::shared_ptr<Node>& node, const std::function<void(const std::shared_ptr<Node>&)>& visitor);

    // 주어진 컨텍스트를 행동 트리에 바인딩
    void Bind(any* context);

public:
    // 행동 트리의 소유자
    weak_ptr<class Monster> owner;

    // 행동 트리의 루트 노드
    shared_ptr<class RootNode> rootNode;

    // 행동 트리에 포함된 모든 노드들을 저장하는 벡터
    vector<shared_ptr<Node>> nodes;

    // 모든 노드들이 공유하는 데이터 저장소인 Blackboard에 대한 포인터
    shared_ptr<Blackboard> blackboard;

    // 행동 트리의 전체 상태 (쓰레드 안전을 위해 atomic 사용)
    atomic<ENodeState> treeState = ENodeState::Running;
};

// Blackboard 클래스는 행동 트리 내의 노드들 간에 데이터를 저장하고 공유하는 역할을 합니다.
class Blackboard : public enable_shared_from_this<Blackboard>
{
public:
    Blackboard() = default;
    virtual ~Blackboard();

public:
    // 행동 트리의 약한 포인터로, 이 Blackboard의 소유 트리를 나타냅니다.
    weak_ptr<BehaviourTree> tree;

private:
    // 읽기/쓰기 락을 사용하여 Blackboard에 대한 접근을 제어하기 위한 잠금 메커니즘
    USE_LOCK;

    // 키가 EBlackboardKey 타입이고 값이 std::any 타입인 맵 (다양한 데이터를 저장 가능)
    map<EBlackboardKey, any> data;

public:
    // 주어진 키와 값을 사용하여 Blackboard에 데이터를 설정합니다.
    void SetData(EBlackboardKey key, any value) 
    {
        WRITE_LOCK; // 쓰기 잠금을 획득하여 쓰레드 안전하게 데이터를 설정
        data[key] = move(value); // 값을 이동하여 저장
    }

    // 주어진 키로부터 Blackboard에서 데이터를 가져옵니다.
    any GetData(EBlackboardKey key) 
    {
        WRITE_LOCK; // 쓰기 잠금을 획득하여 쓰레드 안전하게 데이터를 읽음
        auto it = data.find(key);
        if (it != data.end()) 
        {
            return it->second; // 키가 존재하면 해당 값을 반환
        }
        return GetDefaultValue(key); // 그렇지 않으면 기본값 반환
    }

    // 주어진 키에 대한 기본값을 가져옵니다.
    any GetDefaultValue(EBlackboardKey key)
    {
        switch (key)
        {
        case EBlackboardKey::Target:
            return 0;  // Target의 기본값은 0 (uint64_t로 가정).
        case EBlackboardKey::Position:
            return Vector3{ 0.0f, 0.0f, 0.0f };  // Position의 기본값은 {0, 0, 0}.
        default:
            return {};  // 기본적으로 비어 있는 std::any 반환.
        }
    }
};

// Node 클래스는 행동 트리 내의 단일 노드를 나타냅니다.
class Node : public enable_shared_from_this<Node>
{
public:
    // 생성자는 행동 트리와 Blackboard 포인터로 노드를 초기화합니다.
    Node(shared_ptr<BehaviourTree> _tree, shared_ptr<Blackboard> _blackboard)
        : tree(_tree), blackboard(_blackboard), started(false), state(ENodeState::Running) {}

    virtual ~Node();

    // 노드의 상태를 업데이트하고 그 결과를 반환합니다.
    ENodeState Update()
    {
        // 노드가 아직 시작되지 않았으면 OnStart 호출
        if (!started)
        {
            OnStart(); // 사용자 정의 초기화 로직 실행
            started = true;
        }

        // 노드를 업데이트하고 상태를 설정
        state = OnUpdate();

        // 노드 상태가 더 이상 Running이 아니면 OnStop 호출
        if (state != ENodeState::Running)
        {
            OnStop(); // 노드 실행 완료 시 정리 작업 수행
            started = false;
        }

        return state; // 노드의 현재 상태 반환
    }

    // 노드의 실행을 중단합니다.
    void Abort();

protected:
    // 사용자 정의 동작을 정의하기 위한 가상 함수들 (파생 클래스에서 반드시 구현해야 함)
    virtual void OnStart() = 0;   // 노드가 시작될 때 한 번 호출됨
    virtual void OnStop() = 0;    // 노드가 종료될 때 한 번 호출됨
    virtual ENodeState OnUpdate() = 0; // 노드가 실행 중일 때 반복 호출됨

public:
    // 행동 트리의 약한 포인터 (이 노드의 소유 트리)
    weak_ptr<BehaviourTree> tree;

    // Blackboard에 대한 공유 포인터로, 데이터를 공유하고 액세스하는 데 사용됨
    shared_ptr<Blackboard> blackboard;

    // 부모 노드에 대한 약한 포인터 (이 노드가 자식 노드일 경우)
    weak_ptr<Node> parent;

    // 노드의 현재 상태 (쓰레드 안전을 위해 atomic 사용)
    atomic<ENodeState> state = ENodeState::Running;

    // 노드가 시작되었는지 여부를 나타내는 플래그
    bool started = false;
};

// ServiceNode 클래스는 Node 클래스를 상속받아, 서비스 기능을 수행하는 노드를 나타냅니다.
class ServiceNode : public Node
{
public:
    ServiceNode(shared_ptr<BehaviourTree> _tree, shared_ptr<Blackboard> _blackboard) 
        : Node(_tree, _blackboard) {}
        
    virtual ~ServiceNode();

protected:
    virtual void OnStart() override;
    virtual void OnStop() override;
    virtual ENodeState OnUpdate() override;

    // 서비스 노드의 업데이트를 수행하는 함수로, 파생 클래스에서 구현해야 합니다.
    // 이 함수는 일정한 주기로 호출되어 노드의 특정 서비스 로직을 수행합니다.
    virtual void OnUpdateService() = 0;

protected:
    // 마지막으로 업데이트된 시간 (초 단위)
    float lastUpdateTime = 0.0f;

    // 서비스 노드가 업데이트되는 간격 (초 단위), 기본값은 0.5초
    float updateInterval = 0.5f;

public:
    // 자식 노드를 가리키는 포인터
    // 서비스 노드는 일반적으로 자식 노드를 가지고 있으며, 자식 노드의 실행 상태에 영향을 줄 수 있습니다.
    shared_ptr<Node> child;
};

// DecoratorNode 클래스는 Node 클래스를 상속받아, 행동 트리에서 데코레이터 역할을 하는 노드를 나타냅니다.
// 데코레이터 노드는 하나의 자식 노드를 감싸고, 그 자식의 행동을 변경하거나 제어하는 역할을 수행합니다.
class DecoratorNode : public Node
{
public:
    DecoratorNode(shared_ptr<BehaviourTree> _tree, shared_ptr<Blackboard> _blackboard) 
        : Node(_tree, _blackboard) {}

    virtual ~DecoratorNode();

public:
    // 자식 노드를 가리키는 포인터
    // 데코레이터 노드는 단일 자식 노드를 감싸서 그 자식의 행동을 수정하거나 제어할 수 있습니다.
    shared_ptr<Node> child;
};

// CompositeNode 클래스는 Node 클래스를 상속받아, 여러 자식 노드를 가지는 컴포지트 노드를 나타냅니다.
// 컴포지트 노드는 여러 자식 노드를 실행하여 복잡한 행동을 구성합니다.
class CompositeNode : public Node
{
public:
    CompositeNode(shared_ptr<BehaviourTree> _tree, shared_ptr<Blackboard> _blackboard) 
        : Node(_tree, _blackboard) {}

    virtual ~CompositeNode();

public:
    // 자식 노드들을 저장하는 벡터
    // CompositeNode는 여러 자식 노드를 가지며, 이를 순차적으로 또는 조건부로 실행할 수 있습니다.
    vector<shared_ptr<Node>> children;
};

// SequencerNode 클래스는 CompositeNode 클래스를 상속받아, 자식 노드들을 순차적으로 실행하는 노드를 나타냅니다.
// 모든 자식 노드가 성공할 때까지 순차적으로 실행하며, 중간에 실패하는 노드가 있을 경우 실행을 멈추고 실패 상태를 반환합니다.
class SequencerNode : public CompositeNode
{
public:
    SequencerNode(shared_ptr<BehaviourTree> _tree, shared_ptr<Blackboard> _blackboard) 
        : CompositeNode(_tree, _blackboard) {}

protected:
    virtual void OnStart() override;
    virtual void OnStop() override;
    virtual ENodeState OnUpdate() override;

protected:
    // 현재 실행 중인 자식 노드의 인덱스
    uint32 current;
};

// SelectorNode 클래스는 CompositeNode 클래스를 상속받아, 자식 노드들을 조건부로 실행하는 노드를 나타냅니다.
// 자식 노드를 순차적으로 실행하며, 하나의 자식 노드가 성공하면 나머지 노드들은 실행하지 않고 성공 상태를 반환합니다.
// 모든 자식 노드가 실패할 경우에만 실패 상태를 반환합니다.
class SelectorNode : public CompositeNode
{
public:
    SelectorNode(shared_ptr<BehaviourTree> _tree, shared_ptr<Blackboard> _blackboard) 
        : CompositeNode(_tree, _blackboard) {}

protected:
    virtual void OnStart() override;
    virtual void OnStop() override;
    virtual ENodeState OnUpdate() override;

protected:
    // 현재 실행 중인 자식 노드의 인덱스
    uint32 current;
};

// ActionNode 클래스는 Node 클래스를 상속받아, 행동 트리에서 실제 액션을 수행하는 노드를 나타냅니다.
// 액션 노드는 행동 트리의 리프 노드로, 특정 작업(예: 이동, 공격 등)을 실행합니다.
class ActionNode : public Node
{
public:
    ActionNode(shared_ptr<BehaviourTree> _tree, shared_ptr<Blackboard> _blackboard) 
        : Node(_tree, _blackboard) {}

};
```
### IOCP를 활용한 소켓 송/수신 관리
```{cpp}
 IocpCore::IocpCore()
{
	// IOCP(입출력 완료 포트) 핸들을 생성합니다.
	_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	// 핸들이 유효한지 검사합니다.
	ASSERT_CRASH(_iocpHandle != INVALID_HANDLE_VALUE);
}

IocpCore::~IocpCore()
{
	// 생성된 IOCP 핸들을 닫아줍니다.
	::CloseHandle(_iocpHandle);
}

bool IocpCore::Register(IocpObjectRef iocpObject)
{
	 // IOCP 객체를 등록합니다. 해당 객체의 핸들을 IOCP 핸들과 연결합니다.
	return ::CreateIoCompletionPort(iocpObject->GetHandle(), _iocpHandle, /*key*/0, 0);
}

bool IocpCore::Dispatch(uint32 timeoutMs)
{
	DWORD numOfBytes = 0;
	ULONG_PTR key = 0;	
	IocpEvent* iocpEvent = nullptr;

	// 완료된 IOCP 이벤트를 가져옵니다.
	if (::GetQueuedCompletionStatus(_iocpHandle, OUT &numOfBytes, OUT &key, 
	OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeoutMs))
	{
		// 이벤트가 성공적으로 처리된 경우, 이벤트 소유자를 통해 작업을 처리합니다.
		IocpObjectRef iocpObject = iocpEvent->owner;
		iocpObject->Dispatch(iocpEvent, numOfBytes);
	}
	else
	{
		// 에러 코드 가져오기
		int32 errCode = ::WSAGetLastError();
		switch (errCode)
		{
		case WAIT_TIMEOUT:
			// 타임아웃인 경우 false를 반환하여 더 이상 이벤트가 없음을 알림
			return false;
		default:
			// 오류가 발생했지만 iocpEvent가 유효하다면 작업을 처리합니다.
			IocpObjectRef iocpObject = iocpEvent->owner;
			iocpObject->Dispatch(iocpEvent, numOfBytes);
			break;
		}
	}

	return true;
}
```
### ODBC를 활용한 DB 연결
```{cpp}
bool DBConnection::Connect(SQLHENV environment, const WCHAR* connectionString)
{
	// 데이터베이스 연결 핸들을 할당합니다.
	if (::SQLAllocHandle(SQL_HANDLE_DBC, environment, &_connection) != SQL_SUCCESS)
		return false;

	// 연결 문자열을 복사합니다.
	WCHAR stringBuffer[MAX_PATH] = { 0 };
	::wcscpy_s(stringBuffer, connectionString);

	WCHAR resultString[MAX_PATH] = { 0 };
	SQLSMALLINT resultStringLength = 0;

	// 데이터베이스에 연결합니다.
	SQLRETURN ret = ::SQLDriverConnect(
		_connection,                       // 연결 핸들
    NULL,                              // 윈도우 핸들 (NULL 사용)
    reinterpret_cast<SQLWCHAR*>(stringBuffer), // 연결 문자열
    _countof(stringBuffer),            // 연결 문자열의 길이
    OUT reinterpret_cast<SQLWCHAR*>(resultString), // 결과 문자열
    _countof(resultString),            // 결과 문자열의 길이
    OUT & resultStringLength,          // 결과 문자열의 실제 길이
    SQL_DRIVER_NOPROMPT                // 사용자 입력 없이 연결
	);

	// SQL 문 실행 핸들을 할당합니다.
	if (::SQLAllocHandle(SQL_HANDLE_STMT, _connection, &_statement) != SQL_SUCCESS)
		return false;
		
	// 연결이 성공적으로 완료되었는지 확인합니다.
	return (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO);
}
```
### curl 라이브러리를 사용한 E-mail 인증
```{cpp}
CURLcode Email::Send()
{
    CURLcode ret = CURLE_OK;

		// 수신자 리스트를 저장할 포인터
    struct curl_slist* recipients = NULL;

		// CURL 핸들을 초기화
    CURL* curl = curl_easy_init();

		// 이메일에 포함할 텍스트 데이터를 설정
    StringData textData{ SetPayloadText() };

    if (curl)
    {
        // 발신자 이메일 주소 설정
        curl_easy_setopt(curl, CURLOPT_USERNAME, "hans4809@gmail.com");
        // 발신자 이메일 계정 비밀번호 설정
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "ekyz edrh nphx kvge");
        // SMTP 서버 주소 설정
        curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.gmail.com:465");

        // SSL 사용 설정
        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);

        // 발신자 주소 설정
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, ("<" + _fromAddr + ">").c_str());
        // 수신자 주소 추가
        recipients = curl_slist_append(recipients, ("<" + _toAddr + ">").c_str());
        // 참조 주소 추가
        recipients = curl_slist_append(recipients, ("<" + _ccAddr + ">").c_str());

        // 수신자 리스트 설정
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
        // 이메일 내용을 읽어오기 위한 함수 설정
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
        // 읽어올 데이터 설정
        curl_easy_setopt(curl, CURLOPT_READDATA, &textData);
        // 업로드 옵션 활성화 (이메일 전송)
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        // 자세한 디버그 정보를 출력하도록 설정
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        // 이메일 전송 수행
        ret = curl_easy_perform(curl);

        // 전송이 실패한 경우 오류 메시지 출력
        if (ret != CURLE_OK)
        {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(ret) << "\n";
        }

        // 수신자 리스트 메모리 해제
        curl_slist_free_all(recipients);
        // CURL 핸들 정리
        curl_easy_cleanup(curl);
    }

    return ret; // 전송 결과 반환
}
```
### Command Pattern을 활용한 패킷 처리 로직 (Job)객체화
```{cpp}
class Job
{
public:
	// 람다 또는 함수 객체를 받아와서 Job 객체를 생성하는 생성자
	Job(CallbackType&& callback) : _callback(std::move(callback))
	{
	}

	// 멤버 함수 포인터와 해당 객체를 받아 Job을 생성하는 템플릿 생성자
  // owner: 멤버 함수가 속한 객체의 shared_ptr
  // memFunc: 호출할 멤버 함수 포인터
  // args: 멤버 함수에 전달할 인자들
	template<typename T, typename Ret, typename... Args>
	Job(shared_ptr<T> owner, Ret(T::* memFunc)(Args...), Args... args)
	{
		// 콜백 함수 생성 - owner 객체의 멤버 함수를 호출하는 람다
		_callback = [owner, memFunc, args...]()
		{
			(owner.get()->*memFunc)(args...);
		};
	}

	// 참조 인자를 사용하는 멤버 함수 포인터와 해당 객체를 받아 Job을 생성하는 템플릿 생성자
	template<typename T, typename Ret, typename... Args>
	Job(shared_ptr<T> owner, Ret(T::* memFunc)(Args&...), Args&... args)
	{
		// 콜백 함수 생성 - owner 객체의 멤버 함수를 호출하는 람다
		_callback = [owner, memFunc, args...]()
			{
				(owner.get()->*memFunc)(args...);
			};
	}
	
  // Job 실행 함수 - 저장된 콜백 함수를 실행
	void Execute()
	{
		_callback();
	}

private:
	CallbackType _callback; // 실제 실행할 콜백 함수
};
```
### Command Pattern을 활용한 패킷 처리 로직 (Job)객체화
```{cpp}
void JobQueue::Push(JobRef job, bool pushOnly)
{
	// 현재 작업 수를 증가시키고 이전 작업 수를 저장
	const int32 prevCount = _jobCount.fetch_add(1);
	_jobs.Push(job); // Push에 WRITE_LOCK이 되어있음.

	// 첫번째 Job을 넣은 쓰레드가 실행까지 담당
	if (prevCount == 0)
	{
		// 이미 실행중인 JobQueue가 없으면 실행
		if (LCurrentJobQueue == nullptr && pushOnly == false)
		{
			Execute();
		}
		else
		{
			// 여유 있는 다른 쓰레드가 실행하도록 GlobalQueue에 넘긴다
			GGlobalQueue->Push(shared_from_this());
		}
	}
}

void JobQueue::Execute()
{
	// 현재 JobQueue를 로컬 쓰레드 변수에 저장
	LCurrentJobQueue = this;

	while (true)
	{
		vector<JobRef> jobs;
		// 현재 큐에 있는 모든 Job을 가져옴
		_jobs.PopAll(OUT jobs);

		// 가져온 Job의 수만큼 실행
		const int32 jobCount = static_cast<int32>(jobs.size());
		for (int32 i = 0; i < jobCount; i++)
			jobs[i]->Execute();

		// 남은 일감이 0개라면 종료
		if (_jobCount.fetch_sub(jobCount) == jobCount)
		{
			LCurrentJobQueue = nullptr;
			return;
		}

		const uint64 now = ::GetTickCount64();
		// 현재 시간이 설정된 종료 시간 이후라면 실행 종료 및 GlobalQueue에 넘김
		if (now >= LEndTickCount)
		{
			LCurrentJobQueue = nullptr;
			// 여유 있는 다른 쓰레드가 실행하도록 GlobalQueue에 넘긴다
			GGlobalQueue->Push(shared_from_this());
			break;
		}			
	}
}
```
