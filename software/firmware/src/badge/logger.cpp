#include "logger.h"
#include <diag/Trace.h>

using darknet::LogManager;

LogManager *LogManager::mSelf = 0;

LogManager &LogManager::get() {
	if(0==mSelf) {
		mSelf = new LogManager();
		trace_initialize();
	}
	return *mSelf;
}

LogManager::LogManager() {

}

