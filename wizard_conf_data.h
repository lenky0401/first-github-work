
#ifndef WIZARD_CONF_DATA_H
#define WIZARD_CONF_DATA_H

#include "dummy_config.h"

typedef struct
{
    const gchar* path_prefix;
    const gchar* file_name;
    FcitxConfigFileDesc* cfdesc;
    DummyConfig* config;
} File_Conf_Data;

typedef struct
{
    void* conf_data;
} Wizard_Conf_Data;

GtkWidget* create_assistant(void);

#endif
