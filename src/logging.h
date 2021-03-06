/* Dynvol - Dynamix Volume Toolkit
 * Copyright © 2014-2017 Markus Kitsinger (SwooshyCueb) <root@swooshalicio.us>
 * Copyright © 2014-2017 ΩF:∅ OSWG
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */


#include <string.h>
#include "dynvol.h"
#include "dynvol_private.h"
#include <glib.h>

/* This takes care of g_info not being in glib before 2.40 */
#ifndef g_info
 #if defined(G_HAVE_ISO_VARARGS) && !G_ANALYZER_ANALYZING
  #define g_info(...) g_log (G_LOG_DOMAIN, G_LOG_LEVEL_INFO, __VA_ARGS__)
 #elif defined(G_HAVE_GNUC_VARARGS)  && !G_ANALYZER_ANALYZING
  #define g_info(format...) g_log (G_LOG_DOMAIN, G_LOG_LEVEL_INFO, format)
 #else
  static void g_info (const gchar *format, ...)
  {
    va_list args;
    va_start (args, format);
    g_logv (G_LOG_DOMAIN, G_LOG_LEVEL_INFO, format, args);
    va_end (args);
  }
 #endif
#endif

guint vol_levelmask;
/* Really need to figure something better out for storing this value. */

/* This makes inlusion of line-dependant "macros" easier. */
#define log_error(args...) \
  do { if (vol_levelmask & VOL_LOG_LEVEL_ERROR) { \
    gchar *linestr, *logstr, *fullstr; \
    linestr = g_strdup_printf(__FILE__ ":%d %s  ", __LINE__, \
      __PRETTY_FUNCTION__); \
    logstr  = g_strdup_printf(args); \
    fullstr = g_strconcat(linestr, logstr, NULL); \
    g_error("%s", fullstr); \
    g_free(linestr); g_free(logstr); g_free(fullstr); \
  } } while(0)
#define log_critical(args...) \
  do { if (vol_levelmask & VOL_LOG_LEVEL_CRITICAL) { \
    gchar *linestr, *logstr, *fullstr; \
    linestr = g_strdup_printf(__FILE__ ":%d %s  ", __LINE__, \
      __PRETTY_FUNCTION__); \
    logstr  = g_strdup_printf(args); \
    fullstr = g_strconcat(linestr, logstr, NULL); \
    g_critical("%s", fullstr); \
    g_free(linestr); g_free(logstr); g_free(fullstr); \
  } } while(0)
#define log_warning(args...) \
  do { if (vol_levelmask & VOL_LOG_LEVEL_WARNING) { \
    gchar *linestr, *logstr, *fullstr; \
    linestr = g_strdup_printf(__FILE__ ":%d %s  ", __LINE__, \
      __PRETTY_FUNCTION__); \
    logstr  = g_strdup_printf(args); \
    fullstr = g_strconcat(linestr, logstr, NULL); \
    g_warning("%s", fullstr); \
    g_free(linestr); g_free(logstr); g_free(fullstr); \
  } } while(0)
#define log_message(args...) \
  do { if (vol_levelmask & VOL_LOG_LEVEL_MESSAGE) { \
    gchar *linestr, *logstr, *fullstr; \
    linestr = g_strdup_printf(__FILE__ ":%d %s  ", __LINE__, \
      __PRETTY_FUNCTION__); \
    logstr  = g_strdup_printf(args); \
    fullstr = g_strconcat(linestr, logstr, NULL); \
    g_message("%s", fullstr); \
    g_free(linestr); g_free(logstr); g_free(fullstr); \
  } } while(0)
#define log_info(args...) \
  do { if (vol_levelmask & VOL_LOG_LEVEL_INFO) { \
    gchar *linestr, *logstr, *fullstr; \
    linestr = g_strdup_printf(__FILE__ ":%d %s  ", __LINE__, \
      __PRETTY_FUNCTION__); \
    logstr  = g_strdup_printf(args); \
    fullstr = g_strconcat(linestr, logstr, NULL); \
    g_info("%s", fullstr); \
    g_free(linestr); g_free(logstr); g_free(fullstr); \
  } } while(0)
#define log_debug(args...) \
  do { if (vol_levelmask & VOL_LOG_LEVEL_DEBUG) { \
    gchar *linestr, *logstr, *fullstr; \
    linestr = g_strdup_printf(__FILE__ ":%d %s  ", __LINE__, \
      __PRETTY_FUNCTION__); \
    logstr  = g_strdup_printf(args); \
    fullstr = g_strconcat(linestr, logstr, NULL); \
    g_debug("%s", fullstr); \
    g_free(linestr); g_free(logstr); g_free(fullstr); \
  } } while(0)
#define log_todo(args...) \
  do { if (vol_levelmask & VOL_LOG_LEVEL_TODO) { \
    gchar *linestr, *logstr, *fullstr; \
    linestr = g_strdup_printf(__FILE__ ":%d %s  ", __LINE__, \
      __PRETTY_FUNCTION__); \
    logstr  = g_strdup_printf(args); \
    fullstr = g_strconcat(linestr, logstr, NULL); \
    g_log(G_LOG_DOMAIN, VOL_LOG_LEVEL_TODO, "%s", fullstr); \
    g_free(linestr); g_free(logstr); g_free(fullstr); \
  } } while(0)
#define log_fixme(args...) \
  do { if (vol_levelmask & VOL_LOG_LEVEL_FIXME) { \
    gchar *linestr, *logstr, *fullstr; \
    linestr = g_strdup_printf(__FILE__ ":%d %s  ", __LINE__, \
      __PRETTY_FUNCTION__); \
    logstr  = g_strdup_printf(args); \
    fullstr = g_strconcat(linestr, logstr, NULL); \
    g_log(G_LOG_DOMAIN, VOL_LOG_LEVEL_FIXME, "%s", fullstr); \
    g_free(linestr); g_free(logstr); g_free(fullstr); \
  } } while(0)
#define log_moredebug(args...) \
  do { if (vol_levelmask & VOL_LOG_LEVEL_MOREDEBUG) { \
    gchar *linestr, *logstr, *fullstr; \
    linestr = g_strdup_printf(__FILE__ ":%d %s  ", __LINE__, \
      __PRETTY_FUNCTION__); \
    logstr  = g_strdup_printf(args); \
    fullstr = g_strconcat(linestr, logstr, NULL); \
    g_log(G_LOG_DOMAIN, VOL_LOG_LEVEL_MOREDEBUG, "%s", fullstr); \
    g_free(linestr); g_free(logstr); g_free(fullstr); \
  } } while(0)

/* This is our actual log function */
void logfunc (const gchar *domain, GLogLevelFlags level, const gchar *message,
              gpointer misc);
