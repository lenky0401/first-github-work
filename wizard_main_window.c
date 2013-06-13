#include <stdlib.h>
#include <libintl.h>
#include <errno.h>

#include <fcitx/addon.h>
#include <fcitx-utils/utarray.h>
#include <fcitx-config/fcitx-config.h>
#include <fcitx-config/xdg.h>

#include "config.h"
#include "wizard_main_window.h"
#include "config_widget.h"
#include "configdesc.h"
#include "im_widget.h"
#include "ui_widget.h"

enum {
    LIST_ADDON,
    N_COLUMNS
};

G_DEFINE_TYPE(FcitxWizardMainWindow, fcitx_wizard_main_window, GTK_TYPE_WINDOW)

static void fcitx_wizard_main_window_finalize(GObject* object);

static void
fcitx_wizard_main_window_class_init(FcitxWizardMainWindowClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    gobject_class->finalize = fcitx_wizard_main_window_finalize;
}

GtkWidget*
fcitx_wizard_main_window_new()
{
    FcitxWizardMainWindow* widget =
        g_object_new(FCITX_TYPE_WIZARD_MAIN_WINDOW,
                     NULL);

    return GTK_WIDGET(widget);
}

void fcitx_wizard_main_window_finalize(GObject* object)
{
    FcitxWizardMainWindow* self = FCITX_WIZARD_MAIN_WINDOW(object);
    if (self->addons)
        utarray_free(self->addons);

    G_OBJECT_CLASS (fcitx_wizard_main_window_parent_class)->finalize (object);
}

GtkWidget *page_input_method_engin()
{
    GtkWidget* page_box;
    GtkWidget* imwidget = fcitx_wizard_im_widget_new();
   
    page_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(page_box), imwidget, TRUE, TRUE, 0);
   
    return page_box;

}

GtkWidget *page_personality_skin()
{
    GtkWidget* page_box;
    GtkWidget* imwidget = gtk_image_new_from_file((gchar*)"./002.png");
    
    page_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(page_box), imwidget, TRUE, TRUE, 0);
    
    return page_box;
}

static void
fcitx_wizard_main_window_init(FcitxWizardMainWindow* self)
{
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(self), vbox);

    self->assistant = create_assistant();
    gtk_widget_set_size_request(GTK_WIDGET(self), -1, 400);
    gtk_box_pack_start(GTK_BOX(vbox), self->assistant, TRUE, TRUE, 0);

        
    gtk_window_set_icon_name(GTK_WINDOW(self), "fcitx");
    gtk_window_set_title(GTK_WINDOW(self), _("Input Method Configuration"));
    
    gtk_window_set_position(GTK_WINDOW(self), GTK_WIN_POS_CENTER);
}

GtkWidget *
create_assistant ()
{
    GtkWidget *assistant;
    guint i;
    PageInfo page[PAGE_INFO_NUM] = {
        {NULL, -1, "欢迎使用设置向导", GTK_ASSISTANT_PAGE_INTRO, TRUE},
        {NULL, -1, "输入法引擎选择", GTK_ASSISTANT_PAGE_CONTENT, TRUE},
        {NULL, -1, "个性皮肤选择", GTK_ASSISTANT_PAGE_CONTENT, TRUE},
        {NULL, -1, "候选词栏设置", GTK_ASSISTANT_PAGE_CONTENT, TRUE},
        {NULL, -1, "快捷键设置", GTK_ASSISTANT_PAGE_CONTENT, TRUE},
        {NULL, -1, "向导设置完成", GTK_ASSISTANT_PAGE_CONFIRM, TRUE},
    };

     /* Create a new assistant widget with no pages. */
    assistant = gtk_assistant_new ();
    gtk_widget_set_size_request (assistant, 600, 450);
    gtk_window_set_position (GTK_WINDOW (assistant), GTK_WIN_POS_CENTER);
    gtk_window_set_title (GTK_WINDOW (assistant), "Fcitx配置向导");
    g_signal_connect (G_OBJECT (assistant), "destroy",
                         G_CALLBACK (gtk_main_quit), NULL);

    page[0].widget = gtk_label_new (NULL);
    gchar* s = g_strdup_printf("<b>欢迎使用个性化设置向导！</b>");
    gtk_label_set_markup(GTK_LABEL(page[0].widget), s);

    page[1].widget = page_input_method_engin();

  
    page[2].widget = gtk_label_new ("step 2 test");
    page[3].widget = gtk_label_new ("step 3 test");
    page[4].widget = gtk_label_new ("step 4 test");
    page[5].widget = gtk_label_new ("step 5 test");

    /* Add five pages to the GtkAssistant dialog. */
    for (i = 0; i < PAGE_INFO_NUM; i++) {
        page[i].index = gtk_assistant_append_page (GTK_ASSISTANT (assistant),
                                                      page[i].widget);
        gtk_assistant_set_page_title (GTK_ASSISTANT (assistant),
                                         page[i].widget, page[i].title);
        gtk_assistant_set_page_type (GTK_ASSISTANT (assistant),
                                        page[i].widget, page[i].type);

        /* Set the introduction and conclusion pages as complete so they can be
         * incremented or closed. */
        gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant),
                                            page[i].widget, page[i].complete);
    }
/*
    g_signal_connect (G_OBJECT (assistant), "cancel",
                         G_CALLBACK (assistant_cancel), NULL);
    g_signal_connect (G_OBJECT (assistant), "close",
                         G_CALLBACK (assistant_close), NULL);
    gtk_widget_show_all (assistant);
    gtk_main ();
    return 0;
*/
    return assistant;
}


