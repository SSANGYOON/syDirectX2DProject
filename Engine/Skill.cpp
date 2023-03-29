#include "pch.h"
#include "Skill.h"

Skill::Skill(GameObject* owner)
	:Script(owner), phases(2), summoned(false)
{
}

Skill::~Skill()
{
}

void Skill::Start()
{
}

void Skill::Update()
{
}

void Skill::Summon()
{
}
