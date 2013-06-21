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
#include "wizard_candidate_widget.h"
#include "gdm-languages.h"
#include "config_widget.h"

#include "sub_config_widget.h"
#include "configdesc.h"
#include "dummy_config.h"

enum {
    PROP_0,

    PROP_CONFIG_DESC,
    PROP_PREFIX,
    PROP_NAME,
    PROP_SUBCONFIG,

    PROP_CONFIG_DESC1,
    PROP_PREFIX1,
    PROP_NAME1,
    PROP_SUBCONFIG1
};

G_DEFINE_TYPE(FcitxWizardCandidateWidget, fcitx_wizard_candidate_widget, GTK_TYPE_BOX)

static void fcitx_wizard_candidate_widget_dispose(GObject* object);

static void
fcitx_wizard_candidate_widget_set_property(GObject *gobject,
    guint prop_id, const GValue *value, GParamSpec *pspec);


static GObject *
fcitx_wizard_candidate_widget_constructor(GType gtype, guint n_properties,
    GObjectConstructParam *properties);

static void
fcitx_wizard_candidate_widget_class_init(FcitxWizardCandidateWidgetClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    gobject_class->set_property = fcitx_wizard_candidate_widget_set_property;
    gobject_class->dispose = fcitx_wizard_candidate_widget_dispose;
    gobject_class->constructor = fcitx_wizard_candidate_widget_constructor;

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

    g_object_class_install_property(gobject_class, PROP_CONFIG_DESC1,
        g_param_spec_pointer("cfdesc1", "Configuration Description",
        "Configuration Description for this widget", 
        G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));

    g_object_class_install_property(gobject_class, PROP_PREFIX1,
        g_param_spec_string("prefix1", "Prefix of path",
        "Prefix of configuration path", NULL,
        G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));

    g_object_class_install_property(gobject_class, PROP_NAME1,
        g_param_spec_string("name1", "File name",
        "File name of configuration file", NULL,
        G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));

    g_object_class_install_property(gobject_class, PROP_SUBCONFIG1, 
        g_param_spec_string("subconfig1",
        "subconfig", "subconfig", NULL,
        G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));

}


static GObject *
fcitx_wizard_candidate_widget_constructor(GType gtype,
    guint n_properties, GObjectConstructParam *properties)
{
    GObject *obj;
    obj = G_OBJECT_CLASS(fcitx_wizard_candidate_widget_parent_class)->constructor(
        gtype, n_properties, properties);
    return obj;
}

static void
fcitx_wizard_candidate_widget_init(FcitxWizardCandidateWidget* self)
{

}

static void set_none_font_clicked(GtkWidget *button, gpointer arg)
{
    gtk_font_button_set_font_name(GTK_FONT_BUTTON(arg), "");
}

static void
fcitx_wizard_candidate_widget_load_conf(FcitxWizardCandidateWidget *self)
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
        FcitxLog(WARNING, _("Open file(%s/%s) error.\n"), self->prefix, self->name);
        return;
    }

    dummy_config_load(self->config, fp);
    dummy_config_sync(self->config);

    fclose(fp);

    value = FcitxConfigGetBindValue(&self->config->config, "Output", 
        "CandidateWordNumber");
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(self->candidate_word_number_spin_button), 
        *value.integer);


    if (self->cfdesc1 == NULL) {
        FcitxLog(WARNING, _("Parameter self->cfdesc1 is NULL.\n"));
        return;
    }

    self->config1 = dummy_config_new(self->cfdesc1);

    if ((fp = FcitxXDGGetFileWithPrefix(self->prefix1, self->name1, "r", NULL))
        == NULL) 
    {
        FcitxLog(WARNING, _("Open file(%s/%s) error.\n"), self->prefix1, self->name1);
        return;
    }

    dummy_config_load(self->config1, fp);
    dummy_config_sync(self->config1);

    fclose(fp);

    value = FcitxConfigGetBindValue(&self->config1->config, "ClassicUI", 
        "FontSize");
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(self->font_size_spin_button), 
        *value.integer);

    value = FcitxConfigGetBindValue(&self->config1->config, "ClassicUI", 
        "Font");
    gtk_font_button_set_font_name(GTK_FONT_BUTTON(self->font_button), 
        *value.string);

    value = FcitxConfigGetBindValue(&self->config1->config, "ClassicUI", 
        "VerticalList");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(self->vertical_candidate_button),
        *value.boolvalue); 

}

void 
candidate_word_number_value_changed(GtkWidget* button, 
    gpointer user_data)
{
    FILE *fp;
    GError  *error;
    gchar *argv[3];

    FcitxWizardCandidateWidget *self = user_data;

    self->conf_data.candidate_word_number = 
        gtk_spin_button_get_value(GTK_SPIN_BUTTON(button));
    FcitxConfigBindValue(self->config->config.configFile, "Output", 
        "CandidateWordNumber", 
        &self->conf_data.candidate_word_number, NULL, NULL);

    if ((fp = FcitxXDGGetFileUserWithPrefix(self->prefix, self->name, "w", NULL))
        == NULL) 
    {
        FcitxLog(WARNING, _("Open file(%s/%s) error.\n"), self->prefix, self->name);
        return;
    }

    FcitxConfigSaveConfigFileFp(fp, &self->config->config, self->cfdesc);
    fclose(fp);

    argv[0] = EXEC_PREFIX "/bin/fcitx-remote";
    argv[1] = "-r";
    argv[2] = 0;
    g_spawn_async(NULL, argv, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, &error);
   
}


void 
fcitx_wizard_candidate_widget_conf1_changed(FcitxWizardCandidateWidget *self)
{
    FILE *fp;
    GError  *error;
    gchar *argv[3];
    
    if ((fp = FcitxXDGGetFileUserWithPrefix(self->prefix1, self->name1, "w", NULL))
        == NULL) 
    {
        FcitxLog(WARNING, _("Open file(%s/%s) error.\n"), self->prefix1, self->name1);
        return;
    }

    FcitxConfigSaveConfigFileFp(fp, &self->config1->config, self->cfdesc1);
    fclose(fp);

    argv[0] = EXEC_PREFIX "/bin/fcitx-remote";
    argv[1] = "-r";
    argv[2] = 0;
    g_spawn_async(NULL, argv, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, &error);
   
}

void 
font_size_value_changed(GtkWidget* button, 
    gpointer user_data)
{
    FcitxWizardCandidateWidget *self = user_data;

    self->conf_data.font_size = 
        gtk_spin_button_get_value(GTK_SPIN_BUTTON(button));
    
    FcitxConfigBindValue(self->config1->config.configFile, "ClassicUI", 
        "FontSize", 
        &self->conf_data.font_size, NULL, NULL);

    fcitx_wizard_candidate_widget_conf1_changed(self);

}

void 
font_button_font_set(GtkWidget* button, 
    gpointer user_data)
{
    const char *value;
    FcitxWizardCandidateWidget *self = user_data;

    value = gtk_font_button_get_font_name(GTK_FONT_BUTTON(self->font_button));

    if (self->conf_data.font == NULL &&
        (self->conf_data.font = malloc(FONT_VALUE_MAX_LEN)) == NULL)
    {
        FcitxLog(WARNING, _("Malloc memory(%d) failed.\n"), FONT_VALUE_MAX_LEN);
        return;
    }

    strcpy(self->conf_data.font, value);
    
    FcitxConfigBindValue(self->config1->config.configFile, "ClassicUI", 
        "Font", &self->conf_data.font, NULL, NULL);

    fcitx_wizard_candidate_widget_conf1_changed(self);

}

void 
vertical_candidate_button_toggled(GtkWidget* button, 
    gpointer user_data)
{
    FcitxWizardCandidateWidget *self = user_data;

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button)))
        self->conf_data.vertical_candidate = true;
    else
        self->conf_data.vertical_candidate = false;

    FcitxConfigBindValue(self->config1->config.configFile, "ClassicUI", 
        "VerticalList", &self->conf_data.vertical_candidate, NULL, NULL);

    fcitx_wizard_candidate_widget_conf1_changed(self);

}

static void
fcitx_wizard_candidate_widget_setup_ui(FcitxWizardCandidateWidget *self)
{
    int row = 0;
    GtkWidget *cvbox = GTK_WIDGET(self);
    GtkWidget *hbox;

    GtkWidget *grid = gtk_grid_new();
    gtk_widget_set_margin_left(grid, 0);
    gtk_widget_set_margin_top(grid, 6);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 12);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 6);

    GtkWidget* label = gtk_label_new(("Candidate Word Number"));
    g_object_set(label, "xalign", 0.0f, NULL);
    self->candidate_word_number_spin_button = gtk_spin_button_new_with_range(
            1, 10, 1.0);
    g_object_set(self->candidate_word_number_spin_button, "hexpand", TRUE, NULL);

    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), self->candidate_word_number_spin_button, 1, 
        row, 1, 1);

    row ++;
    label = gtk_label_new(("Font Size"));
    g_object_set(label, "xalign", 0.0f, NULL);
    self->font_size_spin_button = gtk_spin_button_new_with_range(
            0, 72, 1.0);
    g_object_set(self->font_size_spin_button, "hexpand", TRUE, NULL);

    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), self->font_size_spin_button, 1, 
        row, 1, 1);

    row ++;
    label = gtk_label_new(("Font"));
    g_object_set(label, "xalign", 0.0f, NULL);
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    self->font_button = gtk_font_button_new();
    GtkWidget *button = gtk_button_new_with_label(_("Clear font setting"));
    gtk_box_pack_start(GTK_BOX(hbox), self->font_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
    gtk_font_button_set_use_size(GTK_FONT_BUTTON(self->font_button), FALSE);
    gtk_font_button_set_show_size(GTK_FONT_BUTTON(self->font_button), FALSE);
    g_signal_connect(G_OBJECT(button), "clicked", (GCallback) set_none_font_clicked, 
        self->font_button);
    
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), hbox, 1, row, 1, 1);    

    row ++;
    label = gtk_label_new(("Vertical Candidate Word List"));
    g_object_set(label, "xalign", 0.0f, NULL);
    self->vertical_candidate_button = gtk_check_button_new();
    g_object_set(self->vertical_candidate_button, "hexpand", TRUE, NULL);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), self->vertical_candidate_button, 1, row, 1, 1);
    
 
    gtk_box_pack_start(GTK_BOX(cvbox), grid, TRUE, TRUE, 0);

    fcitx_wizard_candidate_widget_load_conf(self);

    g_signal_connect(G_OBJECT(self->candidate_word_number_spin_button), "value-changed",
                 G_CALLBACK(candidate_word_number_value_changed), self);
    g_signal_connect(G_OBJECT(self->font_size_spin_button), "value-changed",
                 G_CALLBACK(font_size_value_changed), self);
    g_signal_connect(G_OBJECT(self->font_button), "font-set",
                 G_CALLBACK(font_button_font_set), self);
    g_signal_connect(G_OBJECT(self->vertical_candidate_button), "toggled",
                 G_CALLBACK(vertical_candidate_button_toggled), self);

}

GtkWidget*
fcitx_wizard_candidate_widget_new(FcitxConfigFileDesc* cfdesc, 
    const gchar* prefix, const gchar* name, const gchar* subconfig,
    FcitxConfigFileDesc* cfdesc1, 
    const gchar* prefix1, const gchar* name1, const gchar* subconfig1)
{
    FcitxWizardCandidateWidget* widget =
        g_object_new(FCITX_TYPE_WIZARD_CANDIDATE_WIDGET,
            "cfdesc", cfdesc, "prefix", prefix, "name", name, 
            "subconfig", subconfig,
            "cfdesc1", cfdesc1, "prefix1", prefix1, "name1", name1, 
            "subconfig1", subconfig1,
             NULL);

    fcitx_wizard_candidate_widget_setup_ui(widget);

    return GTK_WIDGET(widget);
}

void fcitx_wizard_candidate_widget_dispose(GObject* object)
{
    FcitxWizardCandidateWidget* self = FCITX_WIZARD_CANDIDATE_WIDGET(object);
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

    if (self->name1) {
        g_free(self->name1);
        self->name1 = NULL;
    }

    if (self->prefix1) {
        g_free(self->prefix1);
        self->prefix1 = NULL;
    }

    if (self->parser1) {
        sub_config_parser_free(self->parser1);
        self->parser1 = NULL;
    }
 
    if (self->config1) {
        dummy_config_free(self->config1);
        self->config1 = NULL;
    }

    G_OBJECT_CLASS(fcitx_wizard_candidate_widget_parent_class)->dispose(object);
}

static void
fcitx_wizard_candidate_widget_set_property(GObject *gobject,
    guint prop_id, const GValue *value, GParamSpec *pspec)
{
    FcitxWizardCandidateWidget* config_widget = FCITX_WIZARD_CANDIDATE_WIDGET(gobject);
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
    case PROP_CONFIG_DESC1:
        config_widget->cfdesc1 = g_value_get_pointer(value);
        break;
    case PROP_PREFIX1:
        if (config_widget->prefix1)
            g_free(config_widget->prefix1);
        config_widget->prefix1 = g_strdup(g_value_get_string(value));
        break;
    case PROP_NAME1:
        if (config_widget->name1)
            g_free(config_widget->name1);
        config_widget->name1 = g_strdup(g_value_get_string(value));
        break;
    case PROP_SUBCONFIG1:
        if (config_widget->parser1)
            sub_config_parser_free(config_widget->parser1);
        config_widget->parser1 = sub_config_parser_new(g_value_get_string(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, pspec);
        break;
    }
}


