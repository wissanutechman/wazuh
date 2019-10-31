/* Copyright (C) 2015-2019, Wazuh Inc.
 * Copyright (C) 2009 Trend Micro Inc.
 * All right reserved.
 *
 * This program is a free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 2) as published by the FSF - Free Software
 * Foundation
 */

#include "shared.h"
#include "mail-config.h"
#include "config.h"
#include "global-config.h"

int Test_Maild(const char *path, char **output) {
    MailConfig *mail_config;
    _Config *global_config;

    os_calloc(1, sizeof(MailConfig), mail_config);
    os_calloc(1, sizeof(_Config), global_config);

    mail_config->source = -1;

    if(ReadConfig(CMAIL, path, NULL, mail_config, output) < 0) {
        if (output == NULL){
            merror(CONF_READ_ERROR, "Mail options");
        } else {
            wm_strcat(output, "ERROR: Invalid configuration in Mail options", '\n');
        }
        goto fail;
    }
    else if(ReadConfig(CGLOBAL, path, global_config, NULL, output) < 0) {
        if (output == NULL){
            merror(CONF_READ_ERROR, "Mail options");
        } else {
            wm_strcat(output, "ERROR: Invalid configuration in Mail options", '\n');
        }
        goto fail;
    }

    if(mail_config->source == -1) {
        mail_config->source = MAIL_SOURCE_JSON;
    }

    /* If mail config was filled, both log source from mailconfig and globalconfig must match */
    if((mail_config->from || mail_config->smtpserver || mail_config->maxperhour) && mail_config->mn) {
        if((!global_config->alerts_log && !global_config->jsonout_output)) {
            if (output == NULL){
                merror("Mail Config: All alert formats are disabled.");
            } else {
                wm_strcat(output, "ERROR: Invalid configuration in Mail options", '\n');
            }
            goto fail;
        }
        else if((!global_config->alerts_log && (mail_config->source == MAIL_SOURCE_LOGS))) {
            if (output == NULL){
                merror("Mail Config: Alerts.log is disabled when email_log_source selected this log.");
            } else {
                wm_strcat(output, "ERROR: Invalid configuration in Mail options", '\n');
            }
            goto fail;
        }
        else if((!global_config->jsonout_output && (mail_config->source == MAIL_SOURCE_JSON))) {
            if (output == NULL){
                merror("Mail Config: Alerts.json is disabled when email_log_source selected this log.");
            } else {
                wm_strcat(output, "ERROR: Invalid configuration in Mail options", '\n');
            }
            goto fail;
        }
    }

    /* Free memory */
    config_free(global_config);
    freeMailConfig(mail_config);
    if (output) {
        free(global_config);
        free(mail_config);
    }
    return 0;

fail:
    config_free(global_config);
    freeMailConfig(mail_config);
    if (output) {
        free(global_config);
        free(mail_config);
    }
    return OS_INVALID;
}

void freeMailConfig(MailConfig *mailConfig) {
    if(mailConfig) {
        int i;
        os_free(mailConfig->reply_to);
        os_free(mailConfig->from);
        os_free(mailConfig->idsname);
        os_free(mailConfig->smtpserver);
        os_free(mailConfig->heloserver);
        if(mailConfig->to) {
            for (i = 0; mailConfig->to[i]; i++) {
                free(mailConfig->to[i]);
            }
            free(mailConfig->to);
        }

        os_free(mailConfig->gran_level);
        os_free(mailConfig->gran_set);
        os_free(mailConfig->gran_format);
        if(mailConfig->gran_id) {
            for (i = 0;mailConfig->gran_id[i]; i++) {
                free(mailConfig->gran_id[i]);
            }
            free(mailConfig->gran_id);
        }

        if(mailConfig->gran_to) {
            for (i = 0; mailConfig->gran_to[i]; i++) {
                free(mailConfig->gran_to[i]);
            }
            free(mailConfig->gran_to);
        }

        if(mailConfig->gran_location) {
            for (i = 0; mailConfig->gran_location[i]; i++) {
                free(mailConfig->gran_location[i]);
            }
            free(mailConfig->gran_location);
        }

        if(mailConfig->gran_group) {
            forr (i = 0; mailConfig->gran_group[i]; i++) {
                os_free(mailConfig->gran_group[i]);
            }
            free(mailConfig->gran_group);
        }
    }
}