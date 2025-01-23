#pragma once
#include "DB_Class.h"

class DB_MarketService {
public:
	static bool PurchaseMarketItem(int32 buyerId, int32 itemId, shared_ptr<Room> room,int32 slot,int32& retSellerId);
	static bool RegisterMarketItem(int32 sellerId, int32 itemId, int32 price, shared_ptr<ItemDB>& registerItem);
};
