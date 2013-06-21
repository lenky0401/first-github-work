
#ifndef WIZARD_CONF_DATA_H

#define WIZARD_CONF_DATA_H

#define WIZARD_CONF_DATA_CURT_NUM (2)
typedef struct
{
    const gchar* path_prefix;
    const gchar* file_name;
    FcitxConfigFileDesc* cfdesc;
    boolean conf_dirty;
} Wizard_Conf_Data;

GtkWidget* create_assistant(void);

#endif
