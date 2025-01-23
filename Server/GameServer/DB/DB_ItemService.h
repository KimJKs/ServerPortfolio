#pragma once
#include "DB_Class.h"

class DB_ItemService {
public:
    static void AddItem(int32 userId, int32 templateId, int32 count, int32 slot, int32& itemDbId);
    static void UpdateItem(int32 itemDbId, int32 count, bool equipped);
    static void RemoveItem(int32 itemDbId);
};
