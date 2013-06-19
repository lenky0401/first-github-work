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

enum {
    CONFIG_WIDGET_CHANGED,
    LAST_SIGNAL
};

enum {
    PROP_0,

    PROP_CONFIG_DESC,
    PROP_PREFIX,
    PROP_NAME,
    PROP_SUBCONFIG
};
static gint config_widget_signals[LAST_SIGNAL] = { 0 };

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
//gqk
static void
fcitx_wizard_skin_widget_init(FcitxWizardSkinWidget* self)
{
    GError *error = NULL;
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

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(self->dark_skin), TRUE);

/*
    gtk_tool_button_set_icon_widget(GTK_TOOL_BUTTON(self->addimbutton), gtk_image_new_from_gicon(g_themed_icon_new_with_default_fallbacks("list-add-symbolic"), GTK_ICON_SIZE_BUTTON));
    gtk_tool_button_set_icon_widget(GTK_TOOL_BUTTON(self->delimbutton), gtk_image_new_from_gicon(g_themed_icon_new_with_default_fallbacks("list-remove-symbolic"), GTK_ICON_SIZE_BUTTON));
    gtk_tool_button_set_icon_widget(GTK_TOOL_BUTTON(self->moveupbutton), gtk_image_new_from_gicon(g_themed_icon_new_with_default_fallbacks("go-up-symbolic"), GTK_ICON_SIZE_BUTTON));
    gtk_tool_button_set_icon_widget(GTK_TOOL_BUTTON(self->movedownbutton), gtk_image_new_from_gicon(g_themed_icon_new_with_default_fallbacks("go-down-symbolic"), GTK_ICON_SIZE_BUTTON));
//    gtk_tool_button_set_icon_widget(GTK_TOOL_BUTTON(self->configurebutton), gtk_image_new_from_gicon(g_themed_icon_new_with_default_fallbacks("preferences-system-symbolic"), GTK_ICON_SIZE_BUTTON));
//    gtk_tool_button_set_icon_widget(GTK_TOOL_BUTTON(self->default_layout_button), gtk_image_new_from_gicon(g_themed_icon_new_with_default_fallbacks("input-keyboard-symbolic"), GTK_ICON_SIZE_BUTTON));

    g_signal_connect(G_OBJECT(self->addimbutton), "clicked", G_CALLBACK(_fcitx_wizard_im_widget_addim_button_clicked), self);
    g_signal_connect(G_OBJECT(self->delimbutton), "clicked", G_CALLBACK(_fcitx_wizard_im_widget_delim_button_clicked), self);
    g_signal_connect(G_OBJECT(self->moveupbutton), "clicked", G_CALLBACK(_fcitx_wizard_im_widget_moveup_button_clicked), self);
    g_signal_connect(G_OBJECT(self->movedownbutton), "clicked", G_CALLBACK(_fcitx_wizard_im_widget_movedown_button_clicked), self);
//    g_signal_connect(G_OBJECT(self->configurebutton), "clicked", G_CALLBACK(_fcitx_wizard_im_widget_configure_button_clicked), self);
//    g_signal_connect(G_OBJECT(self->default_layout_button), "clicked", G_CALLBACK(_fcitx_wizard_im_widget_default_layout_button_clicked), self);
//    g_signal_connect(G_OBJECT(self->imview), "row-activated", G_CALLBACK(_fcitx_wizard_im_widget_row_activated), self);
*/
}

static void
fcitx_wizard_skin_widget_setup_ui(FcitxWizardSkinWidget *self)
{
    FILE *fp;

    if (self->cfdesc) {
        bindtextdomain(self->cfdesc->domain, LOCALEDIR);
        bind_textdomain_codeset(self->cfdesc->domain, "UTF-8");

        self->config = dummy_config_new(self->cfdesc);
        fp = FcitxXDGGetFileWithPrefix(self->prefix, self->name, "r", NULL);
        if (fp == NULL) {
            //TODO: echo err msg
            return;
        }

        dummy_config_load(self->config, fp);
        dummy_config_sync(self->config);

        fclose(fp);
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

