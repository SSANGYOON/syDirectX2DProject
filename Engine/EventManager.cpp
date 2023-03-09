#include "pch.h"
#include "EventManager.h"

void EventManager::Initialize()
{
	_events.clear();
}

void EventManager::Update()
{
	for (const EventStruct& eve : _events)
	{
		Execute(eve);
	}
	_events.clear();
}

void EventManager::AddEvent(const EventStruct& eve)
{
	_events.push_back(eve);
}

void EventManager::Execute(const EventStruct& es)
{

}
