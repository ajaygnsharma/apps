#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

#include <string>
#include <cstring>
#include <cstdint>

/* Auto-generated file. Do not edit by hand.
 * Module: tx21MIB
 */

#include "tx21MIB_wrappers.h"

int handle_tx21ModelNumber(netsnmp_mib_handler *, netsnmp_handler_registration *, netsnmp_agent_request_info *, netsnmp_request_info *);
int handle_tx21SerialNumber(netsnmp_mib_handler *, netsnmp_handler_registration *, netsnmp_agent_request_info *, netsnmp_request_info *);
int handle_tx21FirmwareVersion(netsnmp_mib_handler *, netsnmp_handler_registration *, netsnmp_agent_request_info *, netsnmp_request_info *);
int handle_tx21HardwareVersion(netsnmp_mib_handler *, netsnmp_handler_registration *, netsnmp_agent_request_info *, netsnmp_request_info *);

static const oid tx21ModelNumber_oid[] = { 1, 3, 6, 1, 4, 1, 21369, 6, 1, 2 };
static const oid tx21SerialNumber_oid[] = { 1, 3, 6, 1, 4, 1, 21369, 6, 1, 3 };
static const oid tx21FirmwareVersion_oid[] = { 1, 3, 6, 1, 4, 1, 21369, 6, 1, 4 };
static const oid tx21HardwareVersion_oid[] = { 1, 3, 6, 1, 4, 1, 21369, 6, 1, 5 };

void init_tx21MIB_scalars(void)
{
    netsnmp_register_scalar(
        netsnmp_create_handler_registration(
            "tx21ModelNumber", handle_tx21ModelNumber,
            tx21ModelNumber_oid, OID_LENGTH(tx21ModelNumber_oid),
            HANDLER_CAN_RONLY
        )
    );

    netsnmp_register_scalar(
        netsnmp_create_handler_registration(
            "tx21SerialNumber", handle_tx21SerialNumber,
            tx21SerialNumber_oid, OID_LENGTH(tx21SerialNumber_oid),
            HANDLER_CAN_RONLY
        )
    );

    netsnmp_register_scalar(
        netsnmp_create_handler_registration(
            "tx21FirmwareVersion", handle_tx21FirmwareVersion,
            tx21FirmwareVersion_oid, OID_LENGTH(tx21FirmwareVersion_oid),
            HANDLER_CAN_RONLY
        )
    );

    netsnmp_register_scalar(
        netsnmp_create_handler_registration(
            "tx21HardwareVersion", handle_tx21HardwareVersion,
            tx21HardwareVersion_oid, OID_LENGTH(tx21HardwareVersion_oid),
            HANDLER_CAN_RONLY
        )
    );

}

int handle_tx21ModelNumber(netsnmp_mib_handler *handler,
             netsnmp_handler_registration *reginfo,
             netsnmp_agent_request_info   *reqinfo,
             netsnmp_request_info         *requests)
{
    (void)handler; (void)reginfo;
    switch(reqinfo->mode) {
            case MODE_GET: 
            case MODE_GETNEXT: {
                std::string s;
                int ret = snmp_scalars_tx21ModelNumber_get(s);
                if (ret != 0) {
                netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_GENERR);
                return SNMP_ERR_NOERROR;
            }
                snmp_set_var_typed_value(
                    requests->requestvb,
                    ASN_OCTET_STR,
                    (const u_char*)s.data(),
                    s.size()
                );
                break;
            }

        default:
            snmp_log(LOG_ERR, "unknown mode (%d) in handle_tx21ModelNumber\\n", reqinfo->mode);
            return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}

int handle_tx21SerialNumber(netsnmp_mib_handler *handler,
             netsnmp_handler_registration *reginfo,
             netsnmp_agent_request_info   *reqinfo,
             netsnmp_request_info         *requests)
{
    (void)handler; (void)reginfo;
    switch(reqinfo->mode) {
            case MODE_GET: 
            case MODE_GETNEXT: {
                std::string s;
                int ret = snmp_scalars_tx21SerialNumber_get(s);
                if (ret != 0) {
                netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_GENERR);
                return SNMP_ERR_NOERROR;
            }
                snmp_set_var_typed_value(
                    requests->requestvb,
                    ASN_OCTET_STR,
                    (const u_char*)s.data(),
                    s.size()
                );
                break;
            }

        default:
            snmp_log(LOG_ERR, "unknown mode (%d) in handle_tx21SerialNumber\\n", reqinfo->mode);
            return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}

int handle_tx21FirmwareVersion(netsnmp_mib_handler *handler,
             netsnmp_handler_registration *reginfo,
             netsnmp_agent_request_info   *reqinfo,
             netsnmp_request_info         *requests)
{
    (void)handler; (void)reginfo;
    switch(reqinfo->mode) {
            case MODE_GET: 
            case MODE_GETNEXT: {
                std::string s;
                int ret = snmp_scalars_tx21FirmwareVersion_get(s);
                if (ret != 0) {
                netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_GENERR);
                return SNMP_ERR_NOERROR;
            }
                snmp_set_var_typed_value(
                    requests->requestvb,
                    ASN_OCTET_STR,
                    (const u_char*)s.data(),
                    s.size()
                );
                break;
            }

        default:
            snmp_log(LOG_ERR, "unknown mode (%d) in handle_tx21FirmwareVersion\\n", reqinfo->mode);
            return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}

int handle_tx21HardwareVersion(netsnmp_mib_handler *handler,
             netsnmp_handler_registration *reginfo,
             netsnmp_agent_request_info   *reqinfo,
             netsnmp_request_info         *requests)
{
    (void)handler; (void)reginfo;
    switch(reqinfo->mode) {
            case MODE_GET: 
            case MODE_GETNEXT: {
                std::string s;
                int ret = snmp_scalars_tx21HardwareVersion_get(s);
                if (ret != 0) {
                netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_GENERR);
                return SNMP_ERR_NOERROR;
            }
                snmp_set_var_typed_value(
                    requests->requestvb,
                    ASN_OCTET_STR,
                    (const u_char*)s.data(),
                    s.size()
                );
                break;
            }

        default:
            snmp_log(LOG_ERR, "unknown mode (%d) in handle_tx21HardwareVersion\\n", reqinfo->mode);
            return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}

static void init_tx21BucInfoTable(void);
static void init_tx21BucAlarmTable(void);
static void init_tx21BucSensorTable(void);
static void init_tx21BucTxTable(void);

/* -------- Table: tx21BucInfoTable -------- */

struct tx21BucInfoTable_row_ctx {
    int idx;
};

static netsnmp_variable_list *
tx21BucInfoTable_get_first(void **loop_ctx, void **data_ctx,
                            netsnmp_variable_list *index, netsnmp_iterator_info *iinfo)
{
    (void)iinfo;
    auto *ctx = new tx21BucInfoTable_row_ctx{ 1 };
    *loop_ctx = ctx;
    *data_ctx = ctx;

    snmp_set_var_typed_integer(index, ASN_INTEGER, ctx->idx);
    return index;
}

static netsnmp_variable_list *
tx21BucInfoTable_get_next(void **loop_ctx, void **data_ctx,
                           netsnmp_variable_list *index, netsnmp_iterator_info *iinfo)
{
    (void)iinfo;
    auto *ctx = (tx21BucInfoTable_row_ctx*)(*loop_ctx);
    if (!ctx) return nullptr;

    ctx->idx++;

    if (ctx->idx > 3) {
        delete ctx;
        *loop_ctx = nullptr;
        *data_ctx = nullptr;
        return nullptr;
    }

    *data_ctx = ctx;
    snmp_set_var_typed_integer(index, ASN_INTEGER, ctx->idx);
    return index;
}

static int
tx21BucInfoTable_handler(netsnmp_mib_handler *handler,
                           netsnmp_handler_registration *reginfo,
                           netsnmp_agent_request_info *reqinfo,
                           netsnmp_request_info *requests)
{
    (void)handler; (void)reginfo;

    for (auto *req = requests; req; req = req->next) {
        auto *tinfo = netsnmp_extract_table_info(req);
        auto *row = (tx21BucInfoTable_row_ctx*)netsnmp_extract_iterator_context(req);

        if (!tinfo || !row) {
            netsnmp_set_request_error(reqinfo, req, SNMP_ERR_GENERR);
            continue;
        }

        const int idx = row->idx;
        const int col = tinfo->colnum;

        if (reqinfo->mode != MODE_GET) {
            netsnmp_set_request_error(reqinfo, req, SNMP_ERR_GENERR);
            continue;
        }

        switch (col) {
            /* no columns */
            default:
                netsnmp_set_request_error(reqinfo, req, SNMP_NOSUCHOBJECT);
                break;
        }
    }

    return SNMP_ERR_NOERROR;
}

static void init_tx21BucInfoTable(void)
{
    static oid base_oid[] = { 1, 3, 6, 1, 4, 1, 21369, 6, 1, 1, 1 };

    netsnmp_handler_registration *reg =
        netsnmp_create_handler_registration("tx21BucInfoTable",
                                            tx21BucInfoTable_handler,
                                            base_oid, OID_LENGTH(base_oid),
                                            HANDLER_CAN_RONLY);

    auto *table_info =
        (netsnmp_table_registration_info*)calloc(1, sizeof(*table_info));
    auto *iinfo =
        (netsnmp_iterator_info*)calloc(1, sizeof(*iinfo));

    netsnmp_table_helper_add_indexes(table_info, ASN_INTEGER, 0);

    table_info->min_column = 2;
    table_info->max_column = 13;

    iinfo->get_first_data_point = tx21BucInfoTable_get_first;
    iinfo->get_next_data_point  = tx21BucInfoTable_get_next;
    iinfo->table_reginfo        = table_info;

    netsnmp_register_table_iterator(reg, iinfo);
}

/* -------- Table: tx21BucAlarmTable -------- */

struct tx21BucAlarmTable_row_ctx {
    int idx;
};

static netsnmp_variable_list *
tx21BucAlarmTable_get_first(void **loop_ctx, void **data_ctx,
                            netsnmp_variable_list *index, netsnmp_iterator_info *iinfo)
{
    (void)iinfo;
    auto *ctx = new tx21BucAlarmTable_row_ctx{ 1 };
    *loop_ctx = ctx;
    *data_ctx = ctx;

    snmp_set_var_typed_integer(index, ASN_INTEGER, ctx->idx);
    return index;
}

static netsnmp_variable_list *
tx21BucAlarmTable_get_next(void **loop_ctx, void **data_ctx,
                           netsnmp_variable_list *index, netsnmp_iterator_info *iinfo)
{
    (void)iinfo;
    auto *ctx = (tx21BucAlarmTable_row_ctx*)(*loop_ctx);
    if (!ctx) return nullptr;

    ctx->idx++;

    if (ctx->idx > 3) {
        delete ctx;
        *loop_ctx = nullptr;
        *data_ctx = nullptr;
        return nullptr;
    }

    *data_ctx = ctx;
    snmp_set_var_typed_integer(index, ASN_INTEGER, ctx->idx);
    return index;
}

static int
tx21BucAlarmTable_handler(netsnmp_mib_handler *handler,
                           netsnmp_handler_registration *reginfo,
                           netsnmp_agent_request_info *reqinfo,
                           netsnmp_request_info *requests)
{
    (void)handler; (void)reginfo;

    for (auto *req = requests; req; req = req->next) {
        auto *tinfo = netsnmp_extract_table_info(req);
        auto *row = (tx21BucAlarmTable_row_ctx*)netsnmp_extract_iterator_context(req);

        if (!tinfo || !row) {
            netsnmp_set_request_error(reqinfo, req, SNMP_ERR_GENERR);
            continue;
        }

        const int idx = row->idx;
        const int col = tinfo->colnum;

        if (reqinfo->mode != MODE_GET) {
            netsnmp_set_request_error(reqinfo, req, SNMP_ERR_GENERR);
            continue;
        }

        switch (col) {
            /* no columns */
            default:
                netsnmp_set_request_error(reqinfo, req, SNMP_NOSUCHOBJECT);
                break;
        }
    }

    return SNMP_ERR_NOERROR;
}

static void init_tx21BucAlarmTable(void)
{
    static oid base_oid[] = { 1, 3, 6, 1, 4, 1, 21369, 6, 2, 10 };

    netsnmp_handler_registration *reg =
        netsnmp_create_handler_registration("tx21BucAlarmTable",
                                            tx21BucAlarmTable_handler,
                                            base_oid, OID_LENGTH(base_oid),
                                            HANDLER_CAN_RONLY);

    auto *table_info =
        (netsnmp_table_registration_info*)calloc(1, sizeof(*table_info));
    auto *iinfo =
        (netsnmp_iterator_info*)calloc(1, sizeof(*iinfo));

    netsnmp_table_helper_add_indexes(table_info, ASN_INTEGER, 0);

    table_info->min_column = 2;
    table_info->max_column = 10;

    iinfo->get_first_data_point = tx21BucAlarmTable_get_first;
    iinfo->get_next_data_point  = tx21BucAlarmTable_get_next;
    iinfo->table_reginfo        = table_info;

    netsnmp_register_table_iterator(reg, iinfo);
}

/* -------- Table: tx21BucSensorTable -------- */

struct tx21BucSensorTable_row_ctx {
    int idx;
};

static netsnmp_variable_list *
tx21BucSensorTable_get_first(void **loop_ctx, void **data_ctx,
                            netsnmp_variable_list *index, netsnmp_iterator_info *iinfo)
{
    (void)iinfo;
    auto *ctx = new tx21BucSensorTable_row_ctx{ 1 };
    *loop_ctx = ctx;
    *data_ctx = ctx;

    snmp_set_var_typed_integer(index, ASN_INTEGER, ctx->idx);
    return index;
}

static netsnmp_variable_list *
tx21BucSensorTable_get_next(void **loop_ctx, void **data_ctx,
                           netsnmp_variable_list *index, netsnmp_iterator_info *iinfo)
{
    (void)iinfo;
    auto *ctx = (tx21BucSensorTable_row_ctx*)(*loop_ctx);
    if (!ctx) return nullptr;

    ctx->idx++;

    if (ctx->idx > 3) {
        delete ctx;
        *loop_ctx = nullptr;
        *data_ctx = nullptr;
        return nullptr;
    }

    *data_ctx = ctx;
    snmp_set_var_typed_integer(index, ASN_INTEGER, ctx->idx);
    return index;
}

static int
tx21BucSensorTable_handler(netsnmp_mib_handler *handler,
                           netsnmp_handler_registration *reginfo,
                           netsnmp_agent_request_info *reqinfo,
                           netsnmp_request_info *requests)
{
    (void)handler; (void)reginfo;

    for (auto *req = requests; req; req = req->next) {
        auto *tinfo = netsnmp_extract_table_info(req);
        auto *row = (tx21BucSensorTable_row_ctx*)netsnmp_extract_iterator_context(req);

        if (!tinfo || !row) {
            netsnmp_set_request_error(reqinfo, req, SNMP_ERR_GENERR);
            continue;
        }

        const int idx = row->idx;
        const int col = tinfo->colnum;

        if (reqinfo->mode != MODE_GET) {
            netsnmp_set_request_error(reqinfo, req, SNMP_ERR_GENERR);
            continue;
        }

        switch (col) {
            /* no columns */
            default:
                netsnmp_set_request_error(reqinfo, req, SNMP_NOSUCHOBJECT);
                break;
        }
    }

    return SNMP_ERR_NOERROR;
}

static void init_tx21BucSensorTable(void)
{
    static oid base_oid[] = { 1, 3, 6, 1, 4, 1, 21369, 6, 3, 1 };

    netsnmp_handler_registration *reg =
        netsnmp_create_handler_registration("tx21BucSensorTable",
                                            tx21BucSensorTable_handler,
                                            base_oid, OID_LENGTH(base_oid),
                                            HANDLER_CAN_RONLY);

    auto *table_info =
        (netsnmp_table_registration_info*)calloc(1, sizeof(*table_info));
    auto *iinfo =
        (netsnmp_iterator_info*)calloc(1, sizeof(*iinfo));

    netsnmp_table_helper_add_indexes(table_info, ASN_INTEGER, 0);

    table_info->min_column = 2;
    table_info->max_column = 7;

    iinfo->get_first_data_point = tx21BucSensorTable_get_first;
    iinfo->get_next_data_point  = tx21BucSensorTable_get_next;
    iinfo->table_reginfo        = table_info;

    netsnmp_register_table_iterator(reg, iinfo);
}

/* -------- Table: tx21BucTxTable -------- */

struct tx21BucTxTable_row_ctx {
    int idx;
};

static netsnmp_variable_list *
tx21BucTxTable_get_first(void **loop_ctx, void **data_ctx,
                            netsnmp_variable_list *index, netsnmp_iterator_info *iinfo)
{
    (void)iinfo;
    auto *ctx = new tx21BucTxTable_row_ctx{ 1 };
    *loop_ctx = ctx;
    *data_ctx = ctx;

    snmp_set_var_typed_integer(index, ASN_INTEGER, ctx->idx);
    return index;
}

static netsnmp_variable_list *
tx21BucTxTable_get_next(void **loop_ctx, void **data_ctx,
                           netsnmp_variable_list *index, netsnmp_iterator_info *iinfo)
{
    (void)iinfo;
    auto *ctx = (tx21BucTxTable_row_ctx*)(*loop_ctx);
    if (!ctx) return nullptr;

    ctx->idx++;

    if (ctx->idx > 3) {
        delete ctx;
        *loop_ctx = nullptr;
        *data_ctx = nullptr;
        return nullptr;
    }

    *data_ctx = ctx;
    snmp_set_var_typed_integer(index, ASN_INTEGER, ctx->idx);
    return index;
}

static int
tx21BucTxTable_handler(netsnmp_mib_handler *handler,
                           netsnmp_handler_registration *reginfo,
                           netsnmp_agent_request_info *reqinfo,
                           netsnmp_request_info *requests)
{
    (void)handler; (void)reginfo;

    for (auto *req = requests; req; req = req->next) {
        auto *tinfo = netsnmp_extract_table_info(req);
        auto *row = (tx21BucTxTable_row_ctx*)netsnmp_extract_iterator_context(req);

        if (!tinfo || !row) {
            netsnmp_set_request_error(reqinfo, req, SNMP_ERR_GENERR);
            continue;
        }

        const int idx = row->idx;
        const int col = tinfo->colnum;

        if (reqinfo->mode != MODE_GET) {
            netsnmp_set_request_error(reqinfo, req, SNMP_ERR_GENERR);
            continue;
        }

        switch (col) {
            case 2: { /* tx21BucTxEnable */
                u_long v = 0;
                int ret = snmp_tx21_buc_tx_enable_get(idx, &v);
                if (ret != 0) {
                    netsnmp_set_request_error(reqinfo, req, SNMP_ERR_GENERR);
                    break;
                }
                snmp_set_var_typed_value(req->requestvb, ASN_INTEGER,
                                         (u_char*)&v, sizeof(v));
                break;
            }

            case 3: { /* tx21BucFrequency */
                u_long v = 0;
                int ret = snmp_tx21_buc_tx_frequency_get(idx, &v);
                if (ret != 0) {
                    netsnmp_set_request_error(reqinfo, req, SNMP_ERR_GENERR);
                    break;
                }
                snmp_set_var_typed_value(req->requestvb, ASN_INTEGER,
                                         (u_char*)&v, sizeof(v));
                break;
            }

            case 4: { /* tx21BucPowerupState */
                u_long v = 0;
                int ret = snmp_tx21_buc_tx_power_up_state_get(idx, &v);
                if (ret != 0) {
                    netsnmp_set_request_error(reqinfo, req, SNMP_ERR_GENERR);
                    break;
                }
                snmp_set_var_typed_value(req->requestvb, ASN_INTEGER,
                                         (u_char*)&v, sizeof(v));
                break;
            }

            case 5: { /* tx21BucPowerupDelay */
                u_long v = 0;
                int ret = snmp_tx21_buc_tx_power_up_delay_get(idx, &v);
                if (ret != 0) {
                    netsnmp_set_request_error(reqinfo, req, SNMP_ERR_GENERR);
                    break;
                }
                snmp_set_var_typed_value(req->requestvb, ASN_INTEGER,
                                         (u_char*)&v, sizeof(v));
                break;
            }

            case 6: { /* tx21BucReadPowerMode */
                u_long v = 0;
                int ret = snmp_tx21_buc_tx_read_power_mode_get(idx, &v);
                if (ret != 0) {
                    netsnmp_set_request_error(reqinfo, req, SNMP_ERR_GENERR);
                    break;
                }
                snmp_set_var_typed_value(req->requestvb, ASN_INTEGER,
                                         (u_char*)&v, sizeof(v));
                break;
            }

            case 7: { /* tx21BucBurstThreshold */
                u_long v = 0;
                int ret = snmp_tx21_buc_tx_burst_thold_mB_get(idx, &v);
                if (ret != 0) {
                    netsnmp_set_request_error(reqinfo, req, SNMP_ERR_GENERR);
                    break;
                }
                snmp_set_var_typed_value(req->requestvb, ASN_INTEGER,
                                         (u_char*)&v, sizeof(v));
                break;
            }

            case 8: { /* tx21BucGainMode */
                u_long v = 0;
                int ret = snmp_tx21_buc_tx_gain_mode_get(idx, &v);
                if (ret != 0) {
                    netsnmp_set_request_error(reqinfo, req, SNMP_ERR_GENERR);
                    break;
                }
                snmp_set_var_typed_value(req->requestvb, ASN_INTEGER,
                                         (u_char*)&v, sizeof(v));
                break;
            }

            case 9: { /* tx21BucAttenuation */
                u_long v = 0;
                int ret = snmp_tx21_buc_tx_attenuation_get(idx, &v);
                if (ret != 0) {
                    netsnmp_set_request_error(reqinfo, req, SNMP_ERR_GENERR);
                    break;
                }
                snmp_set_var_typed_value(req->requestvb, ASN_INTEGER,
                                         (u_char*)&v, sizeof(v));
                break;
            }

            case 10: { /* tx21BucInputTresholdLo */
                u_long v = 0;
                int ret = snmp_tx21_buc_tx_input_threshold_low_get(idx, &v);
                if (ret != 0) {
                    netsnmp_set_request_error(reqinfo, req, SNMP_ERR_GENERR);
                    break;
                }
                snmp_set_var_typed_value(req->requestvb, ASN_INTEGER,
                                         (u_char*)&v, sizeof(v));
                break;
            }

            case 11: { /* tx21BucInputTresholdHi */
                u_long v = 0;
                int ret = snmp_tx21_buc_tx_input_threshold_high_get(idx, &v);
                if (ret != 0) {
                    netsnmp_set_request_error(reqinfo, req, SNMP_ERR_GENERR);
                    break;
                }
                snmp_set_var_typed_value(req->requestvb, ASN_INTEGER,
                                         (u_char*)&v, sizeof(v));
                break;
            }

            case 12: { /* tx21BucOutputTresholdLo */
                u_long v = 0;
                int ret = snmp_tx21_buc_tx_output_threshold_low_get(idx, &v);
                if (ret != 0) {
                    netsnmp_set_request_error(reqinfo, req, SNMP_ERR_GENERR);
                    break;
                }
                snmp_set_var_typed_value(req->requestvb, ASN_INTEGER,
                                         (u_char*)&v, sizeof(v));
                break;
            }

            case 13: { /* tx21BucInputTresholdHi */
                u_long v = 0;
                int ret = snmp_tx21_buc_tx_output_threshold_high_get(idx, &v);
                if (ret != 0) {
                    netsnmp_set_request_error(reqinfo, req, SNMP_ERR_GENERR);
                    break;
                }
                snmp_set_var_typed_value(req->requestvb, ASN_INTEGER,
                                         (u_char*)&v, sizeof(v));
                break;
            }

            case 14: { /* tx21BucBurstTimeout */
                u_long v = 0;
                int ret = snmp_tx21_buc_tx_burst_timeout_get(idx, &v);
                if (ret != 0) {
                    netsnmp_set_request_error(reqinfo, req, SNMP_ERR_GENERR);
                    break;
                }
                snmp_set_var_typed_value(req->requestvb, ASN_INTEGER,
                                         (u_char*)&v, sizeof(v));
                break;
            }

            case 15: { /* tx21BucResetGainControl */
                u_long v = 0;
                int ret = snmp_tx21_buc_tx_reset_gain_control_set(idx, &v);
                if (ret != 0) {
                    netsnmp_set_request_error(reqinfo, req, SNMP_ERR_GENERR);
                    break;
                }
                snmp_set_var_typed_value(req->requestvb, ASN_INTEGER,
                                         (u_char*)&v, sizeof(v));
                break;
            }

            case 16: { /* tx21BucGainControl */
                u_long v = 0;
                int ret = snmp_tx21_buc_tx_gain_control_get(idx, &v);
                if (ret != 0) {
                    netsnmp_set_request_error(reqinfo, req, SNMP_ERR_GENERR);
                    break;
                }
                snmp_set_var_typed_value(req->requestvb, ASN_INTEGER,
                                         (u_char*)&v, sizeof(v));
                break;
            }
            default:
                netsnmp_set_request_error(reqinfo, req, SNMP_NOSUCHOBJECT);
                break;
        }
    }

    return SNMP_ERR_NOERROR;
}

static void init_tx21BucTxTable(void)
{
    static oid base_oid[] = { 1, 3, 6, 1, 4, 1, 21369, 6, 4, 1 };

    netsnmp_handler_registration *reg =
        netsnmp_create_handler_registration("tx21BucTxTable",
                                            tx21BucTxTable_handler,
                                            base_oid, OID_LENGTH(base_oid),
                                            HANDLER_CAN_RONLY);

    auto *table_info =
        (netsnmp_table_registration_info*)calloc(1, sizeof(*table_info));
    auto *iinfo =
        (netsnmp_iterator_info*)calloc(1, sizeof(*iinfo));

    netsnmp_table_helper_add_indexes(table_info, ASN_INTEGER, 0);

    table_info->min_column = 2;
    table_info->max_column = 16;

    iinfo->get_first_data_point = tx21BucTxTable_get_first;
    iinfo->get_next_data_point  = tx21BucTxTable_get_next;
    iinfo->table_reginfo        = table_info;

    netsnmp_register_table_iterator(reg, iinfo);
}

void init_tx21MIB(void)
{
    init_tx21MIB_scalars();
    init_tx21BucInfoTable();
    init_tx21BucAlarmTable();
    init_tx21BucSensorTable();
    init_tx21BucTxTable();
}
