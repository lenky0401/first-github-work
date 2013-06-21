#include <stdlib.h>
#include <libintl.h>
#include <errno.h>

#include <fcitx/addon.h>
#include <fcitx-utils/utarray.h>
#include <fcitx-config/fcitx-config.h>
#include <fcitx-config/xdg.h>

#include "config.h"
#include "config_widget.h"
#include "configdesc.h"
#include "im_widget.h"
#include "ui_widget.h"
#include "wizard_im_widget.h"
#include "wizard_skin_widget.h"
#include "wizard_candidate_widget.h"
#include "wizard_hotkey_widget.h"
#include "wizard_assistant_window.h"
#include "configdesc.h"

Wizard_Conf_Data conf_data[WIZARD_CONF_DATA_CURT_NUM];

static void assistant_cancel(GtkAssistant * assistant, gpointer data);
static void assistant_close(GtkAssistant * assistant, gpointer data);

GtkWidget *
page_input_method_engin(void)
{
    GtkWidget* page_box;
    GtkWidget* imwidget = fcitx_wizard_im_widget_new();
   
    page_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(page_box), imwidget, TRUE, TRUE, 0);
   
    return page_box;

}

GtkWidget *
page_personality_skin(void)
{
    GtkWidget* page_box;
    GtkWidget* imwidget = fcitx_wizard_skin_widget_new(
        get_config_desc("fcitx-classic-ui.desc"), "conf", 
        "fcitx-classic-ui.config", "SkinType");
   
    page_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(page_box), imwidget, TRUE, TRUE, 0);
   
    return page_box;
}

GtkWidget *
page_candidate_word(void)
{
    GtkWidget* page_box;
    GtkWidget* imwidget = fcitx_wizard_candidate_widget_new(
        get_config_desc("config.desc"), "", 
        "config", "Output",
        get_config_desc("fcitx-classic-ui.desc"), "conf", 
        "fcitx-classic-ui.config", "CandidateUi"
        );
   
    page_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(page_box), imwidget, TRUE, TRUE, 0);
   
    return page_box;
}

GtkWidget *
page_hotkey(void)
{
    GtkWidget* page_box;
    GtkWidget* imwidget = fcitx_wizard_hotkey_widget_new(
        get_config_desc("config.desc"), "", 
        "config", "HotKey");
   
    page_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(page_box), imwidget, TRUE, TRUE, 0);
   
    return page_box;
}

boolean load_wizard_conf_data()
{
    int i;
    
    conf_data[0].path_prefix = "";
    conf_data[0].file_name = "config";
    conf_data[0].cfdesc = get_config_desc("config.desc");
    conf_data[0].conf_dirty = false;
    
    conf_data[1].path_prefix = "conf";
    conf_data[1].file_name = "fcitx-classic-ui.config";
    conf_data[1].cfdesc = get_config_desc("fcitx-classic-ui.desc");
    conf_data[1].conf_dirty = false;
    
    for (i = 0; i < WIZARD_CONF_DATA_CURT_NUM; i ++) {
        if (conf_data[1].cfdesc == NULL)
            return false;
    }

    return true;
}

Wizard_Conf_Data conf_data[WIZARD_CONF_DATA_MAX_NUM];


GtkWidget *
create_assistant(void)
{
    guint i;
    GtkWidget *assistant;
    PageInfo page[PAGE_INFO_NUM] = {
        {NULL, -1, "欢迎使用设置向导", GTK_ASSISTANT_PAGE_INTRO, TRUE},
        {NULL, -1, "输入法引擎选择", GTK_ASSISTANT_PAGE_CONTENT, TRUE},
        {NULL, -1, "个性皮肤选择", GTK_ASSISTANT_PAGE_CONTENT, TRUE},
        {NULL, -1, "候选词设置", GTK_ASSISTANT_PAGE_CONTENT, TRUE},
        {NULL, -1, "快捷键设置", GTK_ASSISTANT_PAGE_CONTENT, TRUE},
        {NULL, -1, "向导设置完成", GTK_ASSISTANT_PAGE_CONFIRM, TRUE},
    };

    if (!load_wizard_conf_data()) {
        
    }

    assistant = gtk_assistant_new();
    gtk_widget_set_size_request(assistant, 650, 450);
    gtk_window_set_position(GTK_WINDOW(assistant), GTK_WIN_POS_CENTER);
    gtk_window_set_title(GTK_WINDOW(assistant), "Fcitx配置向导");

    page[0].widget = gtk_label_new(NULL);
    gchar* s = g_strdup_printf("<b>欢迎使用个性化设置向导！</b>");
    gtk_label_set_markup(GTK_LABEL(page[0].widget), s);
    g_free(s);

    page[1].widget = page_input_method_engin();

  
    page[2].widget = page_personality_skin();
    page[3].widget = page_candidate_word();
    page[4].widget = page_hotkey();
    page[5].widget = gtk_label_new(NULL);
    s = g_strdup_printf("<b>个性化设置向导完成，欢迎下次使用！</b>");
    gtk_label_set_markup(GTK_LABEL(page[5].widget), s);
    g_free(s);

    for (i = 0; i < PAGE_INFO_NUM; i++) {
        page[i].index = gtk_assistant_append_page(GTK_ASSISTANT(assistant), 
            page[i].widget);
        
        gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), page[i].widget, 
            page[i].title);
        gtk_assistant_set_page_type(GTK_ASSISTANT(assistant), page[i].widget, 
            page[i].type);

        gtk_assistant_set_page_complete(GTK_ASSISTANT(assistant), page[i].widget, 
            page[i].complete);
    }

    g_signal_connect(G_OBJECT(assistant), "cancel", G_CALLBACK(assistant_cancel), 
        NULL);
    g_signal_connect(G_OBJECT(assistant), "close", G_CALLBACK (assistant_close), 
        NULL);
    
    return assistant;
}

static void
assistant_cancel(GtkAssistant * assistant, gpointer data)
{
    gtk_widget_destroy(GTK_WIDGET(assistant));
}

static void
assistant_close(GtkAssistant * assistant, gpointer data)
{
    gtk_widget_destroy(GTK_WIDGET(assistant));
}

