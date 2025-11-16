#include "LoadAssetThread.h"

LoadAssetThread::LoadAssetThread(int _id,
	IExecutionEvent* callback)
{
	this->id = _id;
	this->onFinished = callback;
}

void LoadAssetThread::run()
{
	TextureManager::getInstance()->
		loadSingleStreamAsset(this->id);
	this->onFinished->OnFinishedExecution();

	delete this;

}