#ifndef PURPLE_PLUGINS
# define PURPLE_PLUGINS
#endif

#include <glib.h>
#include <strings.h>

#ifndef G_GNUC_NULL_TERMINATED
# if __GNUC__ >= 4
#  define G_GNUC_NULL_TERMINATED __attribute__((__sentinel__))
# else
#  define G_GNUC_NULL_TERMINATED
# endif
#endif

//#include "irc.h"
#ifndef IRC_NAMES_FLAG
/* We didn't include irc.h, hack the struct in instead
*/
struct irc_conn {
	gpointer account;
	gpointer msgs;
	gpointer cmds;
	gpointer server;
	int fd;
	guint timer;
	guint who_channel_timer;
};
#endif

#include <connection.h>
#include <plugin.h>
#include <version.h>
#include <debug.h>

static void
remove_irc_who_timeout(PurpleAccount *account)
{
	struct irc_conn *irc;
	PurpleConnection *pc;
	
	pc = purple_account_get_connection(account);
	
	if (!g_str_equal(purple_plugin_get_id(pc->prpl), "prpl-irc"))
		return;
	
	irc = (struct irc_conn *) pc->proto_data;
	
	purple_timeout_remove(irc->who_channel_timer);
	
	purple_debug_info("noircwho", "Removed the /who timeout on IRC account %s\n", purple_account_get_username(account));
}

static void
irc_sending_text(PurpleConnection *pc, gchar **msg, gpointer userdata)
{
	gchar **old = msg;

#define MATCHES(string)   !strncmp(*msg, string, sizeof(string) - 1)

	if (MATCHES("WHO #")) {
		*msg = NULL;
		purple_debug_info("noircwho", "Removed /who on join\n");
	}
	
	if (msg != old)
		g_free(*old);
}

static gboolean
plugin_load (PurplePlugin * plugin)
{
	GList *conns;
	PurplePlugin *ircprpl;
	
	if (purple_major_version != 2)
		return FALSE;
	
	if (purple_minor_version < 4) {
		purple_signal_connect(purple_accounts_get_handle(), "account-signed-on", plugin, PURPLE_CALLBACK(remove_irc_who_timeout), NULL);
		
		for(conns = purple_connections_get_all(); conns; conns = conns->next)
		{
			remove_irc_who_timeout(conns->data);
		}
	}
	
	ircprpl = purple_find_prpl("prpl-irc");
	if (ircprpl != NULL)
		purple_signal_connect(ircprpl, "irc-sending-text", plugin, G_CALLBACK(irc_sending_text), NULL);
	
	return TRUE;
}

static gboolean
plugin_unload (PurplePlugin * plugin)
{
	return TRUE;
}

static PurplePluginInfo info = {
	PURPLE_PLUGIN_MAGIC,
	2,
	10,
	PURPLE_PLUGIN_STANDARD,
	NULL,
	0,
	NULL,
	PURPLE_PRIORITY_DEFAULT,

	"eionrobb-noircwho",
	"No IRC /who",
	"0.2",
	"No IRC /who",
	"No IRC /who",
	"Eion Robb <eionrobb@gmail.com>",
	"",


	plugin_load,
	plugin_unload,
	NULL,

	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

static void
init_plugin (PurplePlugin * plugin)
{
}

PURPLE_INIT_PLUGIN (noircwho, init_plugin, info)
