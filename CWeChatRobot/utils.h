#pragma once
#include <set>
#include <chrono>

#define CHRONO std::chrono
typedef unsigned long long ull;

/*
 * һ���򵥵Ĵ�����ʱ���������ļ��ϣ�intervalΪ����
 * ExpireSet es(1000); // ����ʱ��1��
 * es.CheckIfDuplicatedAndAdd(1); true, δ�ظ�
 * es.CheckIfDuplicatedAndAdd(1); false, �ظ�
 * After 1s;
 * es.CheckIfDuplicatedAndAdd(2); ���<2>��������ڵ�Ԫ��<1>
 */
class ExpireSet
{
public:
    ull interval; // ����
    ExpireSet(ull interval)
    {
        this->interval = interval;
        this->expires_at = 0;
    }
    bool CheckIfDuplicatedAndAdd(ull id)
    {
        bool ok = true;
        if (ids.count(id) != 0)
        {
            ok = false;
        }
        Add(id);
        return ok;
    }

private:
    std::set<ull> ids;
    ull expires_at;

    void Add(ull id)
    {
        // ����
        auto now_ts = CHRONO::time_point_cast<CHRONO::milliseconds>(CHRONO::system_clock::now()).time_since_epoch().count();
#pragma warning(disable : 4018)
        if (expires_at < now_ts)
        {
            ids.clear();
            expires_at = now_ts + interval;
        }
#pragma warning(default : 4018)
        ids.insert(id);
    }
};
