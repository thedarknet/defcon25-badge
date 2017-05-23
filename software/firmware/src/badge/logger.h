#ifndef DCDARK_LOGGER_H
#define DCDARK_LOGGER_H

#include <stdarg.h>
#include <stdint.h>

namespace darknet {
	class Logger;
	static const uint32_t DEBUG_LOGGER_ID = 0;
	static const uint32_t INFO_LOGGER_ID = 1;
	static const uint32_t ERROR_LOGGER_ID = 2;
	static const uint32_t NUMBER_OF_LOGGERS = 3;

	enum LOG_LEVEL {
		LOG_DEBUG_LEVEL = 0
		, LOG_INFO_LEVEL = 1
		, LOG_ERROR_LEVEL = 2
		, LOG_NO_LEVEL = 3
	};
	class LogManager {
	public:
		void setLogLevel(LOG_LEVEL l);
		LOG_LEVEL getLogLevel() const;
		Logger *getLogger(const uint32_t &logID);
		void addLogger(uint32_t id, Logger *l);
	public:
		static LogManager &get();
	protected:
		LogManager();
	private:
		static LogManager *mSelf;
		LOG_LEVEL CurrentLogLevel;
	};
	class Logger {
	public:
		Logger(LOG_LEVEL l);
		const LOG_LEVEL &getLogLevel();
		void printf(const LogManager &lm, const char *format,...);
		virtual ~Logger();
	protected:
		virtual void onPrintf(const char *msg, uint16_t size)=0;
	private:
		LOG_LEVEL CurrentLogLevel;
	};
	class STM32Logger : public darknet::Logger {
	public:
		STM32Logger(LOG_LEVEL l);
	protected:
		virtual void onPrintf(const char *msg, uint16_t size);
	};
}

#define DBGMSG(...) if(LogManager::get().getLogger(darknet::DEBUG_LOGGER_ID)) \
		darknet::LogManager::get().getLogger(darknet::DEBUG_LOGGER_ID)->printf(darknet::LogManager::get(),##__VA_ARGS__)
#define INFOMSG(...) if(darknet::LogManager::get().getLogger(darknet::INFO_LOGGER_ID)) \
		darknet::LogManager::get().getLogger(darknet::INFO_LOGGER_ID)->printf(darknet::LogManager::get(),##__VA_ARGS__)
#define ERRMSG(...) if(LogManager::get().getLogger(darknet::ERROR_LOGGER_ID)) \
		darknet::LogManager::get().getLogger(darknet::ERROR_LOGGER_ID)->printf(darknet::LogManager::get(),##__VA_ARGS__)

#endif
