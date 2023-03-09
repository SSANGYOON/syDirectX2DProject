#pragma once
enum EventType
{
	CREATE_OBJECT,
	SCENE_CHANGE,
	COLLISION,
};

struct EventStruct
{
	EventType type;
	DWORD_PTR lParam;
	DWORD_PTR wParam;
};

class EventManager
{
	DECLARE_SINGLE(EventManager);
public:
	void Initialize();
	void Update();
	void AddEvent(const EventStruct& eve);
private:
	void Execute(const EventStruct& eve);
	
	vector<EventStruct> _events;
};

