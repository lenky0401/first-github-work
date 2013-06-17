/***************************************************************************
 *   Copyright (C) 2010~2012 by CSSlayer                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.              *
 ***************************************************************************/

#include <gtk/gtk.h>
#include <langinfo.h>
#include <libintl.h>
#include <locale.h>
#include "config.h"
#include "config_widget.h"
#include <getopt.h>
#include <gtk/gtk.h>
#include <string.h>
#include "wizard_im_widget.h"
#include "fcitx-config-wizard-gtk3-resources.h"
#include "wizard_assistant_window.h"

typedef GtkApplication FcitxConfigWizardApp;
typedef GtkApplicationClass FcitxConfigWizardAppClass;

G_DEFINE_TYPE (FcitxConfigWizardApp, fcitx_config_wizard_app, GTK_TYPE_APPLICATION)

static void
fcitx_config_wizard_app_activate (GApplication *application)
{
    GList* list = gtk_application_get_windows(GTK_APPLICATION(application));
    if (list) {
        gtk_window_present(GTK_WINDOW(list->data));
    } else {
        GtkWidget *window;
        window = create_assistant();
        gtk_application_add_window(GTK_APPLICATION(application), GTK_WINDOW(window));
        gtk_widget_show_all(GTK_WIDGET(window));
    }
}

static void
fcitx_config_wizard_app_finalize (GObject *object)
{
    G_OBJECT_CLASS(fcitx_config_wizard_app_parent_class)->finalize(object);
}

static void
fcitx_config_wizard_app_init(FcitxConfigWizardApp *app)
{
    fcitx_config_wizard_gtk3_register_resource();
    g_resources_register(fcitx_config_wizard_gtk3_get_resource());
}

int 
fcitx_config_wizard_app_handle_command_line(GApplication *application,
    GApplicationCommandLine *command_line, gpointer user_data)
{
    int argc;
    gchar** argv = g_application_command_line_get_arguments(command_line, &argc);
    g_application_activate(G_APPLICATION(application));
    GList* list = gtk_application_get_windows(GTK_APPLICATION(application));
    if (list) {
        //FcitxWizardMainWindow* mainWindow = FCITX_WIZARD_MAIN_WINDOW (list->data);
        //TODO:
    }

    g_strfreev(argv);
    return 0;
}

static void
fcitx_config_wizard_app_class_init(FcitxConfigWizardAppClass *klass)
{
    G_OBJECT_CLASS(klass)->finalize = fcitx_config_wizard_app_finalize;

    G_APPLICATION_CLASS(klass)->activate = fcitx_config_wizard_app_activate;
}

FcitxConfigWizardApp *
fcitx_config_wizard_app_new(void)
{
#if !GLIB_CHECK_VERSION(2, 35, 1)
    g_type_init();
#endif

    FcitxConfigWizardApp* app = g_object_new(fcitx_config_wizard_app_get_type(),
        "application-id", "org.fcitx.FcitxConfigWizardGtk3",
        "flags", G_APPLICATION_HANDLES_COMMAND_LINE,
        NULL);

    g_signal_connect(app, "command-line", 
        (GCallback)fcitx_config_wizard_app_handle_command_line, NULL);
    
    return app;
}

boolean 
fcitx_config_wizard_process_option(int argc, char* argv[])
{
    struct option longOptions[] = {
        {"wizard", no_argument, NULL, 'W'},
        {NULL, 0, 0, 0}
    };

    int optionIndex = 0;
    int c;
    while ((c = getopt_long(argc, argv, "", longOptions, &optionIndex)) != EOF) {
        switch (c) {
        case 'W':
            //wizardflag = 1;
            break;
        default:
            return false;
        }
    }

    return true;
}

int
main(int argc, char **argv)
{
    setlocale(LC_ALL, "");
    bindtextdomain("fcitx-configtool-wizard", LOCALEDIR);
    bind_textdomain_codeset("fcitx-configtool-wizard", "UTF-8");
    bindtextdomain("fcitx", LOCALEDIR);
    bind_textdomain_codeset("fcitx", "UTF-8");
    textdomain("fcitx-configtool-wizard");
    FcitxLogSetLevel(FCITX_NONE);

    GtkApplication* app = fcitx_config_wizard_app_new();

    int status = 0;
    if (app) {
        GError* error = NULL;
        if (!g_application_register(G_APPLICATION(app), NULL, &error)) {
            g_warning("Cannot register %s", error->message);
            g_error_free(error);
            error = NULL;
        }
        if (g_application_get_is_registered(G_APPLICATION(app))) {
            if (g_application_get_is_remote(G_APPLICATION(app))) {
                g_message("fcitx-config-gtk3 is running.");
            }
        }
    }

    fcitx_config_wizard_process_option(argc, argv);
    status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);

    return status;
}


