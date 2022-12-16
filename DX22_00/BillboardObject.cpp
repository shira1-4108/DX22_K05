#include "BillboardObject.h"
#include "BillboardModel.h"

BillboardObject::BillboardObject()
{
	// ビルボード機能ありのModelクラスをnewする
	mModel = new BillboardModel();
}
