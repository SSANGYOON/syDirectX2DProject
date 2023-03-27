#include "pch.h"
#include "WeaponManager.h"

void WeaponManager::Init()
{
    ifstream json_f;
    json_f.open("c:\\Users\\eondr\\source\\repos\\syDirectX2DProject\\Resources\\PlayerWeapon.json");
    Json::CharReaderBuilder builder;
    builder["collectComments"] = false;
    Json::Value value;

    JSONCPP_STRING errs;
    assert(parseFromStream(builder, json_f, &value, &errs));
    json_f.close();

    auto v = value["Weapons"][0];
    weaponArr[0].frames = v["Frames"].asUInt();
    weaponArr[0].duration = v["Duration"].asFloat();
    auto f = v["Frame Transform"];
    weaponArr[0].transforms.resize(f.size());
    for (int i= 0; i< f.size(); i++)
    {
        weaponArr[0].transforms[i].position = Vector2(f[i]["Offset"][0].asFloat(), f[i]["Offset"][1].asFloat());
        weaponArr[0].transforms[i].angle = f[i]["Angle"].asFloat();
    }

}

WeaponStruct* WeaponManager::GetWeaponStruct(WEAPON_TYPE type)
{
    return &weaponArr[(UINT)type];
}
