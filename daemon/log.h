#ifndef __LOG_H__
#define __LOG_H__


#include <syslog.h>
#include <glib.h>
#include "compat.h"
#include "str.h"



struct call;
struct stream_fd;
struct ice_agent;

struct log_info {
	union {
		struct call *call;
		struct stream_fd *stream_fd;
		const str *str;
		const char *cstr;
		struct ice_agent *ice_agent;
		void *ptr;
	} u;
	enum {
		LOG_INFO_NONE = 0,
		LOG_INFO_CALL,
		LOG_INFO_STREAM_FD,
		LOG_INFO_STR,
		LOG_INFO_C_STRING,
		LOG_INFO_ICE_AGENT,
	} e;
};

extern gboolean _log_stderr;
extern int _log_facility;
extern int _log_facility_cdr;
extern int _log_facility_rtcp;


typedef struct _fac_code {
	char	*c_name;
	int	c_val;
} _fac_code_t;

extern const _fac_code_t _facilitynames[];

typedef void (* write_log_t) (int facility_priority, char *format, ...) __attribute__ ((format (printf, 2, 3)));
extern write_log_t write_log;

void log_to_stderr(int facility_priority, char *format, ...) __attribute__ ((format (printf, 2, 3)));

extern struct log_info __thread log_info;
extern volatile gint log_level;
extern unsigned int max_log_line_length;




void log_init(void);
void __ilog(int prio, const char *fmt, ...)__attribute__ ((format (printf, 2, 3)));
#ifndef __DEBUG
#define ilog(prio, fmt...)									\
	do {											\
		int loglevel = get_log_level();							\
		if (LOG_LEVEL_MASK((prio)) > LOG_LEVEL_MASK(loglevel))				\
			break;									\
		if ((loglevel & LOG_FLAG_RESTORE) && !((prio) & LOG_FLAG_RESTORE))		\
			break;									\
		__ilog(prio, fmt);								\
	} while (0)
#else
#define ilog(prio, fmt...) __ilog(prio, fmt)
#endif

void cdrlog(const char* cdrbuffer);
void rtcplog(const char* cdrbuffer);


INLINE int get_log_level(void) {
	return g_atomic_int_get(&log_level);
}


#define LOG_ERROR LOG_ERR
#define LOG_WARN LOG_WARNING


#define LOG_LEVEL_MASK(v)	((v) & 0x0f)

#define LOG_FLAG_RESTORE	0x10
#define LOG_FLAG_LIMIT		0x20



#endif
