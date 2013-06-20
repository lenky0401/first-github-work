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

#include <fcitx-utils/utils.h>
#include <fcitx/module/dbus/dbusstuff.h>
#include <fcitx/module/ipc/ipc.h>

#include "common.h"
//#include "im_widget.h"
#include "wizard_skin_widget.h"
#include "gdm-languages.h"
#include "config_widget.h"
//#include "main_window.h"
//#include "im_config_dialog.h"
#include "config_widget.h"
#include "keygrab.h"
#include "sub_config_widget.h"
#include "configdesc.h"
#include "dummy_config.h"

//enum {
//    CONFIG_WIDGET_CHANGED,
//    LAST_SIGNAL
//};

enum {
    PROP_0,

    PROP_CONFIG_DESC,
    PROP_PREFIX,
    PROP_NAME,
    PROP_SUBCONFIG
};
//static gint config_widget_signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE(FcitxWizardSkinWidget, fcitx_wizard_skin_widget, GTK_TYPE_BOX)

static void
fcitx_wizard_skin_widget_set_property(GObject *gobject,
    guint prop_id, const GValue *value, GParamSpec *pspec);

static void 
fcitx_wizard_skin_widget_dispose(GObject* object);


static GObject *
fcitx_wizard_skin_widget_constructor(GType gtype,
    guint n_properties, GObjectConstructParam *properties);

static void
fcitx_wizard_skin_widget_class_init(FcitxWizardSkinWidgetClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    gobject_class->set_property = fcitx_wizard_skin_widget_set_property;
    gobject_class->dispose = fcitx_wizard_skin_widget_dispose;
    gobject_class->constructor = fcitx_wizard_skin_widget_constructor;

    g_object_class_install_property(gobject_class, PROP_CONFIG_DESC,
        g_param_spec_pointer("cfdesc", "Configuration Description",
        "Configuration Description for this widget", 
        G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));

    g_object_class_install_property(gobject_class, PROP_PREFIX,
        g_param_spec_string("prefix", "Prefix of path",
        "Prefix of configuration path", NULL,
        G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));

    g_object_class_install_property(gobject_class, PROP_NAME,
        g_param_spec_string("name", "File name",
        "File name of configuration file", NULL,
        G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));

    g_object_class_install_property(gobject_class,
        PROP_SUBCONFIG, g_param_spec_string("subconfig",
        "subconfig", "subconfig", NULL,
        G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
    
}


static GObject *
fcitx_wizard_skin_widget_constructor   (GType                  gtype,
                               guint                  n_properties,
                               GObjectConstructParam *properties)
{
    GObject *obj;
    FcitxWizardSkinWidget *self;
    GtkWidget *widget;

    obj = G_OBJECT_CLASS (fcitx_wizard_skin_widget_parent_class)->constructor (gtype, n_properties, properties);

    self = FCITX_WIZARD_SKIN_WIDGET (obj);

    widget = GTK_WIDGET(gtk_builder_get_object (self->builder,
                                                "skin_widget"));

    gtk_widget_reparent (widget, GTK_WIDGET(self));

  return obj;
}

void 
fcitx_wizard_skin_widget_skin_button_toggled(GtkWidget* button, 
    gpointer user_data)
{
    FILE *fp;
    FcitxWizardSkinWidget *self = user_data;
    GError  *error;
    gchar *argv[3];
    
    if(!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button)))
        return;

    if (self->conf_data.skin_type == NULL &&
        (self->conf_data.skin_type = malloc(SKIN_VALUE_MAX_LEN)) == NULL)
    {
        FcitxLog(WARNING, _("Malloc memory(%d) failed.\n"), SKIN_VALUE_MAX_LEN);
        return;
    }

    if (self->default_skin == button) {
        strcpy(self->conf_data.skin_type, "default");
    } else if (self->classic_skin == button) {
        strcpy(self->conf_data.skin_type, "classic");
    } else if (self->dark_skin == button) {
        strcpy(self->conf_data.skin_type, "dark");
    }

    FcitxConfigBindValue(self->config->config.configFile, "ClassicUI", "SkinType", 
        &self->conf_data.skin_type, NULL, NULL);

    if ((fp = FcitxXDGGetFileUserWithPrefix(self->prefix, self->name, "w", NULL))
        == NULL) 
    {
        FcitxLog(WARNING, _("Open file(%s/%s) error.\n"));
        return;
    }
    FcitxConfigSaveConfigFileFp(fp, &self->config->config, self->cfdesc);
    fclose(fp);

    argv[0] = EXEC_PREFIX "/bin/fcitx-remote";
    argv[1] = "-r";
    argv[2] = 0;
    g_spawn_async(NULL, argv, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, &error);
    
}

static void
fcitx_wizard_skin_widget_init(FcitxWizardSkinWidget* self)
{
    GError *error = NULL;

    self->conf_data.skin_type = NULL;
    
    self->builder = gtk_builder_new();
    gtk_builder_add_from_resource(self->builder, "/org/fcitx/fcitx-config-gtk3/wizard_skin_widget.ui", NULL);

#define _GET_OBJECT(NAME) \
    self->NAME = (typeof(self->NAME)) gtk_builder_get_object(self->builder, #NAME);

    _GET_OBJECT(default_skin)
    _GET_OBJECT(classic_skin)
    _GET_OBJECT(dark_skin)

    gtk_widget_set_size_request(GTK_WIDGET(self->default_skin), 100, 36);
    gtk_button_set_label(GTK_BUTTON(self->default_skin), _("默认"));

    gtk_widget_set_size_request(GTK_WIDGET(self->classic_skin), 100, 36);
    gtk_button_set_label(GTK_BUTTON(self->classic_skin), _("经典"));

    gtk_widget_set_size_request(GTK_WIDGET(self->dark_skin), 100, 36);
    gtk_button_set_label(GTK_BUTTON(self->dark_skin), _("墨黑"));

    _GET_OBJECT(default_skin_img)
    _GET_OBJECT(classic_skin_img)
    _GET_OBJECT(dark_skin_img)

    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file("./default.png", &error);
    gtk_image_set_from_pixbuf(GTK_IMAGE(self->default_skin_img), pixbuf);
    g_object_unref(pixbuf);

    pixbuf = gdk_pixbuf_new_from_file("./classic.png", &error);
    gtk_image_set_from_pixbuf(GTK_IMAGE(self->classic_skin_img), pixbuf);
    g_object_unref(pixbuf);

    pixbuf = gdk_pixbuf_new_from_file("./dark.png", &error);
    gtk_image_set_from_pixbuf(GTK_IMAGE(self->dark_skin_img), pixbuf);
    g_object_unref(pixbuf);

    g_signal_connect(G_OBJECT(self->default_skin), "toggled", 
        G_CALLBACK(fcitx_wizard_skin_widget_skin_button_toggled), self);
    g_signal_connect(G_OBJECT(self->classic_skin), "toggled", 
        G_CALLBACK(fcitx_wizard_skin_widget_skin_button_toggled), self);
    g_signal_connect(G_OBJECT(self->dark_skin), "toggled", 
        G_CALLBACK(fcitx_wizard_skin_widget_skin_button_toggled), self);

}

static void
fcitx_wizard_skin_widget_setup_ui(FcitxWizardSkinWidget *self)
{
    FILE *fp;
    FcitxConfigValueType value;

    if (self->cfdesc == NULL) {
        FcitxLog(WARNING, _("Parameter self->cfdesc is NULL.\n"));
        return;
    }

    bindtextdomain(self->cfdesc->domain, LOCALEDIR);
    bind_textdomain_codeset(self->cfdesc->domain, "UTF-8");

    self->config = dummy_config_new(self->cfdesc);
    
    if ((fp = FcitxXDGGetFileWithPrefix(self->prefix, self->name, "r", NULL))
        == NULL) 
    {
        FcitxLog(WARNING, _("Open file(%s/%s) error.\n"));
        return;
    }

    dummy_config_load(self->config, fp);
    dummy_config_sync(self->config);

    fclose(fp);

    value = FcitxConfigGetBindValue(&self->config->config, "ClassicUI", "SkinType");
    FcitxLog(DEBUG, _("SkinType:%s.\n"), *(value.string));

    if (strcmp(*(value.string), "default") == 0) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(self->default_skin), TRUE);
    } else if (strcmp(*(value.string), "classic") == 0) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(self->classic_skin), TRUE);
    } else if (strcmp(*(value.string), "dark") == 0) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(self->dark_skin), TRUE);
    } else {
        FcitxLog(WARNING, _("SkinType error.\n"));
    }

}

GtkWidget*
fcitx_wizard_skin_widget_new(FcitxConfigFileDesc* cfdesc, const gchar* prefix, 
    const gchar* name, const gchar* subconfig)
{
    FcitxWizardSkinWidget* widget = g_object_new(FCITX_TYPE_WIZARD_SKIN_WIDGET, 
        "cfdesc", cfdesc, "prefix", 
        prefix, "name", name, "subconfig", subconfig, NULL);

    fcitx_wizard_skin_widget_setup_ui(widget);

    return GTK_WIDGET(widget);
}

void fcitx_wizard_skin_widget_dispose(GObject* object)
{
    FcitxWizardSkinWidget* self = FCITX_WIZARD_SKIN_WIDGET(object);
    if (self->name) {
        g_free(self->name);
        self->name = NULL;
    }

    if (self->prefix) {
        g_free(self->prefix);
        self->prefix = NULL;
    }

    if (self->parser) {
        sub_config_parser_free(self->parser);
        self->parser = NULL;
    }
 
    if (self->config) {
        dummy_config_free(self->config);
        self->config = NULL;
    }
 
    G_OBJECT_CLASS (fcitx_wizard_skin_widget_parent_class)->dispose (object);
}

static void
fcitx_wizard_skin_widget_set_property(GObject *gobject,
    guint prop_id, const GValue *value, GParamSpec *pspec)
{
    FcitxWizardSkinWidget* config_widget = FCITX_WIZARD_SKIN_WIDGET(gobject);
    switch (prop_id) {
    case PROP_CONFIG_DESC:
        config_widget->cfdesc = g_value_get_pointer(value);
        break;
    case PROP_PREFIX:
        if (config_widget->prefix)
            g_free(config_widget->prefix);
        config_widget->prefix = g_strdup(g_value_get_string(value));
        break;
    case PROP_NAME:
        if (config_widget->name)
            g_free(config_widget->name);
        config_widget->name = g_strdup(g_value_get_string(value));
        break;
    case PROP_SUBCONFIG:
        if (config_widget->parser)
            sub_config_parser_free(config_widget->parser);
        config_widget->parser = sub_config_parser_new(g_value_get_string(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, pspec);
        break;
    }
}

