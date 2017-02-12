#ifndef DCDARK_LOGGER_H
#define DCDARK_LOGGER_H

namespace darknet {
	class LogManager {
	public:
		static LogManager &get();
	protected:
		LogManager();
	private:
		static LogManager *mSelf;
	};
}

#define DBGMSG(a)
#define INFOMSG(a)
#define ERRMSG(a)

#endif
