/*  -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */

/*\
|*|
|*| nautilus-open-sublime.c
|*|
|*| https://github.com/phucnoob/nautilus-open-sublime
|*|
|*| Copyright (C) 2022 Pham Van Phuc <phuclaplace@gmail.com>
|*|
|*| **Nautilus Open Sublime** is free software: you can redistribute it and/or
|*| modify it under the terms of the GNU General Public License as published by
|*| the Free Software Foundation, either version 3 of the License, or (at your
|*| option) any later version.
|*|
|*| **Nautilus Open Sublime** is distributed in the hope that it will be useful,
|*| but WITHOUT ANY WARRANTY; without even the implied warranty of
|*| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
|*| Public License for more details.
|*|
|*| You should have received a copy of the GNU General Public License along
|*| with this program. If not, see <http://www.gnu.org/licenses/>.
|*|
\*/



#include <stdlib.h>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <glib.h>
#include <nautilus-extension.h>



/*\
|*|
|*| BUILD SETTINGS
|*|
\*/


#ifdef ENABLE_NLS
#include <glib/gi18n-lib.h>
#define I18N_INIT() \
	bindtextdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
#else
#define _(STRING) ((char * ) STRING)
#define I18N_INIT()
#endif



/*\
|*|
|*| GLOBAL TYPES AND VARIABLES
|*|
\*/


typedef struct {
	GObject parent_slot;
} NautilusOpenSublime;

typedef struct {
	GObjectClass parent_slot;
} NautilusOpenSublimeClass;

static GType provider_types[1];
static GType nautilus_open_sublime_type;
static GObjectClass * parent_class;



/*\
|*|
|*| FUNCTIONS
|*|
\*/

static void sublime_open_directory(gchar *path) {
	// TODO
	g_message("Opening: %s", path);
	const char *command = g_strdup_printf("$(which subl) %s", path);
	system(command);

	g_free(path);
}

static void on_do_stuff_selected_menuitem_activate (
	NautilusMenuItem * const menu_item,
	gpointer const user_data
) {

	/*

		This is the function that is invoked when the user clicks on the "Do
		something" menu entry.

	*/
	GFile * file;

	GList * const file_selection = g_object_get_data(
		G_OBJECT(menu_item),
		"nautilus_open_sublime_files"
	);

	gchar *all_files = "";
	// gchar *space = " ";

	for (GList * iter = file_selection; iter; iter = iter->next) {

		/*  Launch Nautilus from a terminal to see this  */
		gchar * file_uri;
		gchar * file_path;

		file_uri = nautilus_file_info_get_uri(NAUTILUS_FILE_INFO(iter->data));
		file = nautilus_file_info_get_location(NAUTILUS_FILE_INFO(iter->data));

		if (!G_IS_FILE(file))
		{
			g_warning("Unable to get %s location", file_uri);
			continue;
		}

		if ((file_uri = g_file_get_path(file)) == NULL)
		{
			g_warning("The file %s doesn\'t exists", file_uri);
			continue;
		}

		file_path = g_file_get_path(file);
		// Encap the path with quote '' to avoid space filename.
		all_files = g_strconcat(all_files , "\'",file_path, "\'", " ", NULL);

		// g_free(file);
		g_free(file_path);
		g_free(file_uri);

	}

	// g_free(space);
	sublime_open_directory(all_files);

}


static void on_do_stuff_background_menuitem_activate (
	NautilusMenuItem * const menu_item,
	gpointer const user_data
) {

	/*

		This is the function that is invoked when the user clicks on the "Do
		something" menu entry. Good coding! ;)

	*/

	// gchar * file_uri;
	GFile *file;

	NautilusFileInfo * const background_item = g_object_get_data(
		G_OBJECT(menu_item),
		"nautilus_open_sublime_background_items"
	);


	file = nautilus_file_info_get_location(background_item);

	if (!G_IS_FILE(file))
	{
		g_warning("Not sellecting a file...Exit.");
		return;
	}

	gchar *path;
	if ((path = g_file_get_path(file)) == NULL)
	{
		g_warning("Unable to get selected item path...Exit.");
		return;
	}
	sublime_open_directory(path);

	g_free(path);
}


static GList * nautilus_open_sublime_get_file_items (
	NautilusMenuProvider * const menu_provider,
	GtkWidget * const nautilus_window,
	GList * const file_selection
) {

	/*

	Uncomment the following block if you want the "Do something" menu entry not
	to be shown if a directory is found among the selected files.

	If you want instead to show the menu entry only when a particular mimetype
	is selected, use as condition:
	`!nautilus_file_is_mime_type(NAUTILUS_FILE_INFO(iter->data), "application/x-[MIME TYPE]")`

	For further information see the `NautilusFileInfo` interface at:
	https://developer.gnome.org/libnautilus-extension/stable/NautilusFileInfo.html

	*/

	/*

	for (GList * iter = file_selection; iter; iter = iter->next) {

		if (nautilus_file_info_is_directory(NAUTILUS_FILE_INFO(iter->data))) {

			return NULL;

		}

	}

	*/

	NautilusMenuItem * const menu_item = nautilus_menu_item_new(
		"NautilusOpenSublime::do_stuff_selected",
		_("Open in Sublime"),
		_("Open this item in sublime."),
		NULL /* icon name or `NULL` */
	);

	g_signal_connect(
		menu_item,
		"activate",
		G_CALLBACK(on_do_stuff_selected_menuitem_activate),
		NULL /* or any custom user data */
	);

	g_object_set_data_full(
		G_OBJECT(menu_item),
		"nautilus_open_sublime_files",
		nautilus_file_info_list_copy(file_selection),
		(GDestroyNotify) nautilus_file_info_list_free
	);

	return g_list_append(NULL, menu_item);

}


static GList * nautilus_open_sublime_get_background_items (
	NautilusMenuProvider * const menu_provider,
	GtkWidget * const nautilus_window,
	NautilusFileInfo * const current_folder
) {

	NautilusMenuItem * const menu_item = nautilus_menu_item_new(
		"NautilusOpenSublime::do_stuff_background",
		_("Open in Sublime"),
		_("Open current directory in Sublime"),
		NULL    /*  icon name or `NULL`  */
	);

	g_signal_connect(
		menu_item,
		"activate",
		G_CALLBACK(on_do_stuff_background_menuitem_activate),
		NULL    /*  or any custom user data  */
	);

	g_object_ref(current_folder);

	g_object_set_data_full(
		G_OBJECT(menu_item),
		"nautilus_open_sublime_background_items",
		current_folder,
		(GDestroyNotify) g_object_unref
	);

	return g_list_append(NULL, menu_item);

}


static void nautilus_open_sublime_menu_provider_iface_init (
	NautilusMenuProviderIface * const iface,
	gpointer const iface_data
) {

	iface->get_file_items = nautilus_open_sublime_get_file_items;
	iface->get_background_items = nautilus_open_sublime_get_background_items;

}


static void nautilus_open_sublime_class_init (
	NautilusOpenSublimeClass * const nautilus_open_sublime_class,
	gpointer class_data
) {

	parent_class = g_type_class_peek_parent(nautilus_open_sublime_class);

}


static void nautilus_open_sublime_register_type (
	GTypeModule * const module
) {

	static const GTypeInfo info = {
		sizeof(NautilusOpenSublimeClass),
		(GBaseInitFunc) NULL,
		(GBaseFinalizeFunc) NULL,
		(GClassInitFunc) nautilus_open_sublime_class_init,
		(GClassFinalizeFunc) NULL,
		NULL,
		sizeof(NautilusOpenSublime),
		0,
		(GInstanceInitFunc) NULL,
		(GTypeValueTable *) NULL
	};

	nautilus_open_sublime_type = g_type_module_register_type(
		module,
		G_TYPE_OBJECT,
		"NautilusOpenSublime",
		&info,
		0
	);

	static const GInterfaceInfo menu_provider_iface_info = {
		(GInterfaceInitFunc) nautilus_open_sublime_menu_provider_iface_init,
		(GInterfaceFinalizeFunc) NULL,
		NULL
	};

	g_type_module_add_interface(
		module,
		nautilus_open_sublime_type,
		NAUTILUS_TYPE_MENU_PROVIDER,
		&menu_provider_iface_info
	);

}


GType nautilus_open_sublime_get_type (void) {

	return nautilus_open_sublime_type;

}


void nautilus_module_shutdown (void) {

	/*  Any module-specific shutdown  */

}


void nautilus_module_list_types (
	const GType ** const types,
	int * const num_types
) {

	*types = provider_types;
	*num_types = G_N_ELEMENTS(provider_types);

}


void nautilus_module_initialize (
	GTypeModule * const module
) {

	I18N_INIT();
	nautilus_open_sublime_register_type(module);
	*provider_types = nautilus_open_sublime_get_type();

}


/*  EOF  */

