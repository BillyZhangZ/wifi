/*
 * Copyright (c) 2014 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
// Copyright (c) 2011 Qualcomm Atheros, Inc.
// All Rights Reserved.
// Qualcomm Atheros Confidential and Proprietary.
// Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is
// hereby granted, provided that the above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
// INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
// USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#ifndef UTILITY_H
#define UTILITY_H

typedef struct {
    /** log level:
     *  0 = notice:  logNote()
     *  1 = normal:  logNorm()
     *  2 = verbose: logVerb()
     *  3 = debug:   logDebug()
     */
    char level;
    /** True to used syslog instead of console */
    char useSyslog;
    /** Bit mask of items to log (according to LogMask) */
    int mask;
    char *timeStampFormat;
    /** Name of error file where all ERROR messages are written to */
    char *errorFilename;
    /** Name used for syslog */
    char *syslogName;
} logConfig_t;

void logInit(const char *appName, char useSyslog, char level, int mask);

void logGetConfig(logConfig_t *logConfig);
void logSetConfig(const logConfig_t *logConfig);

int logGetMask(void);

void logSysError(const char *format, ...);
void logError(const char *format, ...);
void logWarn(const char *format, ...);

void logMasked(int flag, const char *format, ...);

void logNote(const char *format, ...);
void logNorm(const char *format, ...);
void logVerb(const char *format, ...);
void logDebug(const char *format, ...);
void logTrace(const char *format, ...);

void logHexStr(int level, const char *message, const void *buf, unsigned int len);

void hexdump(const void *addr, unsigned int len, unsigned char columns);

int toHexString(char *buf, size_t size, const void *addr, size_t len, char insertSpace);
int fromHexString(void *outBuf, size_t size, const char *hexstr);

void dieSysError(const char *format, ...);
void dieError(const char *format, ...);

void logToErrorFile(const char *msg);
int logTruncateErrorfile(int minNumLines, int maxNumLines);

void strToUpper(char *str);
void strToLower(char *str);

#endif // UTILITY_H
