#include "logger.h"
#include <diag/Trace.h>
#include <stdio.h>

using darknet::LogManager;
using darknet::Logger;
using darknet::LOG_LEVEL;

LogManager *LogManager::mSelf = 0;

LogManager &LogManager::get() {
	if(0==mSelf) {
		mSelf = new LogManager();
		trace_initialize();
	}
	return *mSelf;
}

LogManager::LogManager() : CurrentLogLevel(LOG_LEVEL::LOG_NO_LEVEL){

}

void LogManager::setLogLevel(LOG_LEVEL l) {
	CurrentLogLevel = l;
}

LOG_LEVEL LogManager::getLogLevel() const {
	return CurrentLogLevel;
}

static Logger *Loggers[darknet::NUMBER_OF_LOGGERS] = {0};

void LogManager::addLogger(uint32_t id, Logger *l) {
	if(id<darknet::NUMBER_OF_LOGGERS) return;
	Loggers[id] = l;
}

Logger *LogManager::getLogger(const uint32_t &id) {
	if(id>=darknet::NUMBER_OF_LOGGERS) return 0;
	return Loggers[id];
}

////////////////////////////////////////

Logger::Logger(LOG_LEVEL l) : CurrentLogLevel(l) {

}

const LOG_LEVEL &Logger::getLogLevel() {
	return CurrentLogLevel;
}

void Logger::printf(const LogManager &lm, const char *format,...) {
	if(lm.getLogLevel()<=getLogLevel()) {
		va_list ap;
		va_start (ap, format);
		static char buf[64];
		int ret = vsnprintf (buf, sizeof(buf), format, ap);
		if(ret>0) {
			onPrintf(buf,(uint16_t)ret);
		}
		va_end (ap);
	}
}


Logger::~Logger() {

}



darknet::STM32Logger::STM32Logger(LOG_LEVEL l) : Logger(l) {

}

void darknet::STM32Logger::onPrintf(const char *msg, uint16_t size) {
	trace_write (msg, (unsigned int)size);
}
