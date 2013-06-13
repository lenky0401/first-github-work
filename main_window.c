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

#include <stdlib.h>
#include <libintl.h>
#include <errno.h>

#include <fcitx/addon.h>
#include <fcitx-utils/utarray.h>
#include <fcitx-config/fcitx-config.h>
#include <fcitx-config/xdg.h>

#include "config.h"
#include "main_window.h"
#include "config_widget.h"
#include "configdesc.h"
#include "im_widget.h"
#include "ui_widget.h"

enum {
    LIST_ADDON,
    N_COLUMNS
};

G_DEFINE_TYPE(FcitxMainWindow, fcitx_main_window, GTK_TYPE_WINDOW)

static void fcitx_main_window_finalize(GObject* object);

static void _fcitx_main_window_add_config_file_page(FcitxMainWindow* self);

static void _fcitx_main_window_add_addon_page(FcitxMainWindow* self);

static void _fcitx_main_window_add_im_page(FcitxMainWindow* self);

static void _fcitx_main_window_add_ui_page(FcitxMainWindow* self);

void _fcitx_main_window_add_page(FcitxMainWindow* self, const char* name, GtkWidget* widget, const char* stock);

static void _fcitx_main_window_addon_selection_changed(GtkTreeSelection *selection, gpointer data);

static void _fcitx_main_window_addon_row_activated(GtkTreeView       *tree_view,
                                                   GtkTreePath       *path,
                                                   GtkTreeViewColumn *column,
                                                   gpointer           user_data);

static void _fcitx_main_window_configure_button_clicked(GtkButton *button, gpointer data);

static void _fcitx_main_window_enabled_data_func(GtkCellLayout   *cell_layout,
        GtkCellRenderer *renderer,
        GtkTreeModel    *tree_model,
        GtkTreeIter     *iter,
        gpointer         user_data);

static void _fcitx_main_window_name_data_func(GtkCellLayout   *cell_layout,
        GtkCellRenderer *renderer,
        GtkTreeModel    *tree_model,
        GtkTreeIter     *iter,
        gpointer         user_data);

static void _fcitx_main_window_toggled_cb(GtkCellRenderer *renderer,
        gchar* str_path,
        gpointer         user_data);

const UT_icd addonicd = {sizeof(FcitxAddon), 0, 0, FcitxAddonFree};

static void
fcitx_main_window_class_init(FcitxMainWindowClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    gobject_class->finalize = fcitx_main_window_finalize;
}

//gqk
void _fcitx_main_window_add_wizard_page_step_1(FcitxMainWindow* self)
{

}

void _fcitx_main_window_add_wizard_page_step_2(FcitxMainWindow* self)
{

}

void _fcitx_main_window_add_wizard_page_step_3(FcitxMainWindow* self)
{

}

void _fcitx_main_window_add_wizard_page_step_4(FcitxMainWindow* self)
{

}

void _fcitx_main_window_add_wizard_page_step_5(FcitxMainWindow* self)
{

}

void _fcitx_main_window_add_wizard_page_step_6(FcitxMainWindow* self)
{

}

static void entry_changed (GtkEditable*, GtkAssistant*);
       static void button_toggled (GtkCheckButton*, GtkAssistant*);
       static void button_clicked (GtkButton*, GtkAssistant*);
       static void assistant_cancel (GtkAssistant*, gpointer);
       static void assistant_close (GtkAssistant*, gpointer);



         /* If there is text in the GtkEntry, set the page as complete. Otherwise,
        * stop the user from progressing the next page. */
        static void
        entry_changed (GtkEditable *entry,
                                  GtkAssistant *assistant)
        {
            const gchar *text = gtk_entry_get_text (GTK_ENTRY (entry));
            gint num = gtk_assistant_get_current_page (assistant);
            GtkWidget *page = gtk_assistant_get_nth_page (assistant, num);
            gtk_assistant_set_page_complete (assistant, page, (strlen (text) > 0));
        }

        /* If the check button is toggled, set the page as complete. Otherwise,
        * stop the user from progressing the next page. */
        static void
        button_toggled (GtkCheckButton *toggle,
                                  GtkAssistant *assistant)
        {
            gboolean active = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (toggle));
            gtk_assistant_set_page_complete (assistant, GTK_WIDGET (toggle), active);
        }

        /* Fill up the progress bar, 10% every second when the button is clicked. Then,
        * set the page as complete when the progress bar is filled. */
        static void
        button_clicked (GtkButton *button,
                                 GtkAssistant *assistant)
        {
            GtkProgressBar *progress;
            GtkWidget *page;
            gdouble percent = 0.0;
            gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);
            page = gtk_assistant_get_nth_page (assistant, 3);
            progress = GTK_PROGRESS_BAR (g_object_get_data (G_OBJECT (page), "pbar"));
            while (percent <= 100.0)
            {
                gchar *message = g_strdup_printf ("%.0f%% Complete", percent);
                gtk_progress_bar_set_fraction (progress, percent / 100.0);
                gtk_progress_bar_set_text (progress, message);
                while (gtk_events_pending ())
                gtk_main_iteration ();
                g_usleep (500000);
                percent += 5.0;
            }
            gtk_assistant_set_page_complete (assistant, page, TRUE);
        }

        /* If the dialog is cancelled, delete it from memory and then clean up after
        * the Assistant structure. */
        static void
        assistant_cancel (GtkAssistant *assistant,
                                    gpointer data)
        {
            gtk_widget_destroy (GTK_WIDGET (assistant));
        }

        /* This function is where you would apply the changes and destroy the assistant. */
        static void
        assistant_close (GtkAssistant *assistant,
                                   gpointer data)
        {
            g_print ("You would apply your changes now!\n");
            gtk_widget_destroy (GTK_WIDGET (assistant));
        }


//gqk
//extern int wizardflag;
static void
fcitx_main_window_init(FcitxMainWindow* self)
{
    int i;
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

 //   if (wizardflag == 0) {
        self->configNotebook = gtk_notebook_new();
        _fcitx_main_window_add_im_page(self);
        _fcitx_main_window_add_config_file_page(self);
        _fcitx_main_window_add_ui_page(self);
        _fcitx_main_window_add_addon_page(self);
    
        gtk_widget_set_size_request(GTK_WIDGET(self), -1, 400);
        gtk_box_pack_start(GTK_BOX(vbox), self->configNotebook, TRUE, TRUE, 0);
 //   } else {
    /*
        for (i = 0; i < WIZARD_STEP_NUM; i ++) {
            self->wizard_box[i] = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
            gtk_box_pack_start(GTK_BOX(vbox), self->wizard_box[i], TRUE, TRUE, 0);
            
            GtkWidget* top_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
            GtkWidget* center_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
            GtkWidget* bottom_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

            gtk_box_pack_start(GTK_BOX(self->wizard_box[i]), top_box, TRUE, TRUE, 0);
            gtk_box_pack_start(GTK_BOX(self->wizard_box[i]), center_box, TRUE, TRUE, 0);
            gtk_box_pack_start(GTK_BOX(self->wizard_box[i]), bottom_box, TRUE, TRUE, 0);

            gchar* text_tips = g_strdup_printf("step %d, test ...             "
                "\t\t\t\t\t\t\t\t\t\t\t\t\t", (i + 1));
            GtkWidget *plabel = gtk_label_new(text_tips);
            gtk_box_pack_start(GTK_BOX(top_box), plabel, TRUE, TRUE, 0);

            GtkWidget* imwidget = gtk_button_new_with_label("page1");
            gtk_box_pack_start(GTK_BOX(center_box), imwidget, TRUE, TRUE, 0);

            gtk_widget_hide(GTK_WIDGET(self->wizard_box[i]));
            gtk_widget_hide(GTK_WIDGET(top_box));
            gtk_widget_hide(GTK_WIDGET(center_box));
            gtk_widget_hide(GTK_WIDGET(bottom_box));
            gtk_widget_hide(GTK_WIDGET(plabel));
            gtk_widget_hide(GTK_WIDGET(imwidget));
            gtk_widget_hide(GTK_WIDGET(bottom_box));

        }
*/

    //gtk_notebook_set_show_tabs(self->configNotebook)


    gtk_container_add(GTK_CONTAINER(self), vbox);
    gtk_window_set_icon_name(GTK_WINDOW(self), "fcitx");
    gtk_window_set_title(GTK_WINDOW(self), _("Input Method Configuration"));
    
    gtk_window_set_position(GTK_WINDOW(self), GTK_WIN_POS_CENTER);
}

GtkWidget*
fcitx_main_window_new()
{
    FcitxMainWindow* widget =
        g_object_new(FCITX_TYPE_MAIN_WINDOW,
                     NULL);

    return GTK_WIDGET(widget);
}

void fcitx_main_window_finalize(GObject* object)
{
    FcitxMainWindow* self = FCITX_MAIN_WINDOW(object);
    if (self->addons)
        utarray_free(self->addons);

    G_OBJECT_CLASS (fcitx_main_window_parent_class)->finalize (object);
}

void _fcitx_main_window_add_page(FcitxMainWindow* self, const char* name, GtkWidget* widget, const char* stock)
{
    gtk_widget_show_all(widget);

    GtkWidget *plabel = gtk_label_new(name);
    gtk_notebook_append_page(GTK_NOTEBOOK(self->configNotebook),
                                widget,
                                plabel);
}

void _fcitx_main_window_addon_selection_changed(GtkTreeSelection *selection, gpointer data)
{
    GtkTreeView *treeView = gtk_tree_selection_get_tree_view(selection);
    GtkTreeModel *model = gtk_tree_view_get_model(treeView);
    GtkTreeIter iter;
    FcitxAddon *addon = NULL;
    FcitxMainWindow* self = data;

    if (!self->button)
        return;

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gtk_tree_model_get(model, &iter,
                           LIST_ADDON, &addon,
                           -1);
        gchar* config_desc_name = g_strdup_printf("%s.desc", addon->name);
        FcitxConfigFileDesc* cfdesc = get_config_desc(config_desc_name);
        g_free(config_desc_name);
        gboolean configurable = (gboolean)(cfdesc != NULL || strlen(addon->subconfig) != 0);
        gtk_widget_set_sensitive(self->button, configurable);
    } else {
        gtk_widget_set_sensitive(self->button, FALSE);
    }
}

void _fcitx_main_window_addon_row_activated(GtkTreeView* tree_view, GtkTreePath* path, GtkTreeViewColumn* column, gpointer user_data)
{
    GtkTreeModel *model = gtk_tree_view_get_model(tree_view);
    GtkTreeIter iter;
    FcitxAddon *addon = NULL;
    if (gtk_tree_model_get_iter(model, &iter, path)) {
        gtk_tree_model_get(model, &iter,
                           LIST_ADDON, &addon,
                           -1);

        GtkWidget* dialog = fcitx_config_dialog_new(addon, GTK_WINDOW(user_data));
        if (dialog)
            gtk_widget_show_all(GTK_WIDGET(dialog));
    }
}
//gqk
static void
_fcitx_main_window_config_widget_changed(FcitxConfigWidget* widget, gpointer user_data)
{
    fcitx_config_widget_response(widget, CONFIG_WIDGET_SAVE);
}

void _fcitx_main_window_add_config_file_page(FcitxMainWindow* self)
{
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    FcitxConfigWidget* config_widget = fcitx_config_widget_new(
                                           get_config_desc("config.desc"),
                                           "",
                                           "config",
                                           NULL
                                       );
    g_object_set(G_OBJECT(config_widget), "margin", 5, NULL);
    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(config_widget), TRUE, TRUE, 0);

    GtkWidget* hbuttonbox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(vbox), hbuttonbox, FALSE, TRUE, 0);
    g_object_set(G_OBJECT(hbuttonbox), "margin", 5, NULL);

    g_signal_connect(config_widget, "changed", (GCallback) _fcitx_main_window_config_widget_changed, NULL);

    _fcitx_main_window_add_page(self, _("Global Config"), vbox, GTK_STOCK_PREFERENCES);
}

void _fcitx_main_window_add_im_page(FcitxMainWindow* self)
{
    GtkWidget* imwidget = fcitx_im_widget_new();
    _fcitx_main_window_add_page(self, _("Input Method"), imwidget, GTK_STOCK_EDIT);
}

gboolean _ui_connect(gpointer user_data)
{
    fcitx_ui_widget_connect(FCITX_UI_WIDGET(user_data));
    return false;
}

void _fcitx_main_window_add_ui_page(FcitxMainWindow* self)
{
    GtkWidget* uiwidget = fcitx_ui_widget_new();
    _fcitx_main_window_add_page(self, _("Appearance"), uiwidget, GTK_STOCK_ABOUT);

    g_idle_add_full(G_PRIORITY_DEFAULT_IDLE, (GSourceFunc)_ui_connect, g_object_ref(uiwidget), (GDestroyNotify) g_object_unref);
}

gboolean _filter_addon_func(GtkTreeModel *model,
                            GtkTreeIter  *iter,
                            gpointer      data)
{
    FcitxMainWindow* self = data;
    FcitxAddon* addon = NULL;
    const gchar* filter_text = gtk_entry_get_text(GTK_ENTRY(self->filterentry));
    gtk_tree_model_get(model,
                       iter,
                       LIST_ADDON, &addon,
                       -1);

    gboolean flag = TRUE;
    if (addon) {
        if ((addon->category == AC_FRONTEND || !addon->bEnabled || addon->advance) && !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(self->advancecheckbox))) {
            return false;
        }
        flag = flag && (strlen(filter_text) == 0
                 || strstr(addon->name, filter_text)
                 || strstr(addon->comment, filter_text)
                 || strstr(addon->generalname, filter_text));
    }
    return flag;
}

void _fcitx_main_window_checkbox_changed(GtkToggleButton* button, gpointer user_data)
{
    FcitxMainWindow* self = user_data;
    gtk_tree_view_column_set_visible(self->checkboxcolumn, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(self->advancecheckbox)));
    gtk_tree_model_filter_refilter(GTK_TREE_MODEL_FILTER(self->filtermodel));
}

static void
icon_press_cb (GtkEntry       *entry,
               gint            position,
               GdkEventButton *event,
               gpointer        data)
{
    gtk_entry_set_text (entry, "");
}

void _fcitx_main_window_filtertext_changed(GtkEditable* editable, gpointer user_data)
{
    FcitxMainWindow* self = user_data;
    gtk_tree_model_filter_refilter(GTK_TREE_MODEL_FILTER(self->filtermodel));
}
//gqk
void _fcitx_main_window_add_addon_page(FcitxMainWindow* self)
{
    /* load addon */
    FcitxAddon* addon;
    utarray_new(self->addons, &addonicd);
    FcitxAddonsLoad(self->addons);

    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    /* advance check box */
    self->advancecheckbox = gtk_check_button_new_with_label(_("Advance"));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(self->advancecheckbox), FALSE);
    g_signal_connect(G_OBJECT(self->advancecheckbox), "toggled", G_CALLBACK(_fcitx_main_window_checkbox_changed), self);

    /* filter entry */
    self->filterentry = gtk_entry_new();
    gtk_entry_set_icon_from_stock (GTK_ENTRY (self->filterentry),
                                    GTK_ENTRY_ICON_SECONDARY,
                                    GTK_STOCK_CLEAR);
    g_object_set(G_OBJECT(self->filterentry), "margin", 5, NULL);
#if GTK_CHECK_VERSION(3,2,0)
    gtk_entry_set_placeholder_text(GTK_ENTRY (self->filterentry), _("Search Addon"));
#endif
    g_signal_connect(G_OBJECT(self->filterentry), "icon-press", G_CALLBACK (icon_press_cb), NULL);

    /* list view */
    self->addonstore = gtk_list_store_new(N_COLUMNS, G_TYPE_POINTER);
    for (addon = (FcitxAddon *) utarray_front(self->addons);
         addon != NULL;
         addon = (FcitxAddon *) utarray_next(self->addons, addon)) {
        GtkTreeIter iter;
        gtk_list_store_append(self->addonstore, &iter);
        gtk_list_store_set(self->addonstore, &iter, LIST_ADDON, addon, -1);
    }

    self->filtermodel = gtk_tree_model_filter_new(GTK_TREE_MODEL(self->addonstore), NULL);

    gtk_tree_model_filter_set_visible_func(GTK_TREE_MODEL_FILTER(self->filtermodel),
                                           (GtkTreeModelFilterVisibleFunc) _filter_addon_func,
                                           self,
                                           NULL);
    self->addonview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(self->filtermodel));

    /* add column check box */
    self->togglecell = gtk_cell_renderer_toggle_new();
    self->checkboxcolumn = gtk_tree_view_column_new_with_attributes("Enable", self->togglecell, NULL);
    gtk_tree_view_column_set_visible(self->checkboxcolumn, FALSE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(self->addonview), self->checkboxcolumn);
    gtk_cell_layout_set_cell_data_func(GTK_CELL_LAYOUT(self->checkboxcolumn),
                                       self->togglecell,
                                       _fcitx_main_window_enabled_data_func,
                                       NULL,
                                       NULL);

    /* add column text */
    GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn* column = gtk_tree_view_column_new_with_attributes("Name", renderer, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(self->addonview), column);
    gtk_cell_layout_set_cell_data_func(GTK_CELL_LAYOUT(column),
                                       renderer,
                                       _fcitx_main_window_name_data_func,
                                       NULL,
                                       NULL);

    /* add addon list to vbox */
    GtkWidget* swin = gtk_scrolled_window_new(NULL, NULL);
    g_object_set(swin, "hscrollbar-policy", GTK_POLICY_NEVER, NULL);
    g_object_set(self->addonview, "headers-visible", FALSE, NULL);
    gtk_container_add(GTK_CONTAINER(swin), self->addonview);
    gtk_box_pack_start(GTK_BOX(vbox), swin, TRUE, TRUE, 0);
    g_object_set(G_OBJECT(swin), "margin-left", 5, "margin-right", 5, "shadow-type", GTK_SHADOW_IN, NULL);

    g_signal_connect(G_OBJECT(self->togglecell), "toggled",
                     G_CALLBACK(_fcitx_main_window_toggled_cb), GTK_TREE_MODEL(self->addonstore));

    gtk_box_pack_start(GTK_BOX(vbox), self->advancecheckbox, FALSE, TRUE, 0);
    g_object_set(G_OBJECT(self->advancecheckbox), "margin-left", 5, "margin-right", 5, NULL);
    gtk_box_pack_start(GTK_BOX(vbox), self->filterentry, FALSE, TRUE, 5);

    /* configure button */
    GtkWidget* hbuttonbox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(vbox), hbuttonbox, FALSE, TRUE, 0);
    g_object_set(G_OBJECT(hbuttonbox), "margin", 5, NULL);

    self->button = gtk_button_new_with_label(_("Configure"));
    gtk_widget_set_sensitive(self->button, FALSE);
    gtk_button_set_image(GTK_BUTTON(self->button), gtk_image_new_from_stock(GTK_STOCK_PREFERENCES, GTK_ICON_SIZE_BUTTON));
    gtk_box_pack_start(GTK_BOX(hbuttonbox), self->button, TRUE, TRUE, 0);
    g_signal_connect(G_OBJECT(self->button), "clicked", G_CALLBACK(_fcitx_main_window_configure_button_clicked), self);

    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(self->addonview));
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_BROWSE);
    g_signal_connect(G_OBJECT(selection), "changed",
                     G_CALLBACK(_fcitx_main_window_addon_selection_changed), self);
    g_signal_connect(G_OBJECT(self->filterentry), "changed", G_CALLBACK(_fcitx_main_window_filtertext_changed), self);
    gtk_tree_model_filter_refilter(GTK_TREE_MODEL_FILTER(self->filtermodel));

    g_signal_connect(G_OBJECT(self->addonview),
                     "row-activated",
                     G_CALLBACK(_fcitx_main_window_addon_row_activated), self);

    _fcitx_main_window_add_page(self, _("Addon"), vbox, GTK_STOCK_ADD);
}

static void
_fcitx_main_window_toggled_cb(GtkCellRenderer *renderer,
                              gchar* str_path,
                              gpointer         user_data)
{
    GtkTreeModel *model = (GtkTreeModel *)user_data;
    GtkTreePath *path = gtk_tree_path_new_from_string(str_path);
    GtkTreeIter iter;
    gtk_tree_model_get_iter(model, &iter, path);
    FcitxAddon* addon = NULL;
    gtk_tree_path_free(path);
    gtk_tree_model_get(model,
                       &iter,
                       LIST_ADDON, &addon,
                       -1);

    if (!addon)
        return;

    addon->bEnabled = !addon->bEnabled;
    char *buf;
    asprintf(&buf, "%s.conf", addon->name);
    FILE* fp = FcitxXDGGetFileUserWithPrefix("addon", buf, "w", NULL);
    free(buf);
    if (fp) {
        fprintf(fp, "[Addon]\nEnabled=%s\n", addon->bEnabled ? "True" : "False");
        fclose(fp);
    }
    g_object_set(renderer,
                 "active", (gboolean) addon->bEnabled,
                 NULL);
}

static void
_fcitx_main_window_enabled_data_func(GtkCellLayout   *cell_layout,
                                     GtkCellRenderer *renderer,
                                     GtkTreeModel    *tree_model,
                                     GtkTreeIter     *iter,
                                     gpointer         user_data)
{
    FcitxAddon* addon = NULL;

    gtk_tree_model_get(tree_model,
                       iter,
                       LIST_ADDON, &addon,
                       -1);

    g_object_set(renderer,
                "active", addon->bEnabled,
                NULL);
}

static void
_fcitx_main_window_name_data_func(GtkCellLayout   *cell_layout,
                                  GtkCellRenderer *renderer,
                                  GtkTreeModel    *tree_model,
                                  GtkTreeIter     *iter,
                                  gpointer         user_data)
{
    FcitxAddon* addon;

    gtk_tree_model_get(tree_model,
                       iter,
                       LIST_ADDON, &addon,
                       -1);
    gchar* string = g_strdup_printf("%s\n%s", addon->generalname, addon->comment);
    g_object_set(renderer,
                 "text", string,
                 NULL);

    g_free(string);
}

void _fcitx_main_window_configure_button_clicked(GtkButton* button, gpointer data)
{
    FcitxMainWindow* self = data;
    GtkTreeView* view = GTK_TREE_VIEW(self->addonview);
    GtkTreeSelection *selection = gtk_tree_view_get_selection(view);
    GtkTreeModel *model = gtk_tree_view_get_model(view);
    GtkTreeIter iter;
    FcitxAddon *addon = NULL;
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gtk_tree_model_get(model, &iter,
                           LIST_ADDON, &addon,
                           -1);

        GtkWidget* dialog = fcitx_config_dialog_new(addon, GTK_WINDOW(self));
        if (dialog)
            gtk_widget_show_all(GTK_WIDGET(dialog));
    }
}
