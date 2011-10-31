#ifndef PURPLE_PLUGINS
# define PURPLE_PLUGINS
#endif

#include <glib.h>

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
remove_irc_who_timeout(PurpleConnection *pc)
{
	struct irc_conn *irc;
	
	if (!g_str_equal(purple_plugin_get_id(pc->prpl), "prpl-irc"))
		return;
	if (purple_connection_get_state(pc) != PURPLE_CONNECTED)
		return;
	
	irc = (struct irc_conn *) pc->proto_data;
	
	purple_timeout_remove(irc->who_channel_timer);
	
	purple_debug_info("noircwho", "Removed the /who timeout on IRC account %s\n", purple_account_get_username(purple_connection_get_account(pc)));
}

static gboolean
plugin_load (PurplePlugin * plugin)
{
	GList *conns;
	
	purple_signal_connect(purple_connections_get_handle(), "signed-on", plugin, PURPLE_CALLBACK(remove_irc_who_timeout), NULL);
	
	for(conns = purple_connections_get_all(); conns; conns = conns->next)
	{
		remove_irc_who_timeout(conns->data);
	}
	
	return TRUE;
}

static gboolean
plugin_unload (PurplePlugin * plugin)
{
	return TRUE;
}

static PurplePluginInfo info = {
	PURPLE_PLUGIN_MAGIC,
	PURPLE_MAJOR_VERSION,
	PURPLE_MINOR_VERSION,
	PURPLE_PLUGIN_STANDARD,
	NULL,
	0,
	NULL,
	PURPLE_PRIORITY_DEFAULT,

	"eionrobb-noircwho",
	"No IRC /who",
	"0.1",
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
