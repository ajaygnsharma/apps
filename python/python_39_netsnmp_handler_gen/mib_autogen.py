#!/usr/bin/env python3
import csv
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Optional, List, Dict, Any, Tuple, DefaultDict
from collections import defaultdict

#from distro import info

# ---------------- Types supported in generation ----------------

ASN_INFO: Dict[str, Dict[str, Any]] = {
    "ASN_INTEGER": {
        "ctype": "u_long",
        "check": "ASN_INTEGER",
        "set_typed": "ASN_INTEGER",
        "get_from_vb": "*(requests->requestvb->val.integer)",
        "vb_value_len": "sizeof(value)",
        "value_ptr": "(u_char *)&value",
        "is_string": False,
    },
    "ASN_IPADDRESS": {
        "ctype": "u_long",
        "check": "ASN_IPADDRESS",
        "set_typed": "ASN_IPADDRESS",
        "get_from_vb": "*(requests->requestvb->val.integer)",
        "vb_value_len": "sizeof(value)",
        "value_ptr": "(u_char *)&value",
        "is_string": False,
    },
    "ASN_OCTET_STR": {
        "ctype": "std::string",
        "check": "ASN_OCTET_STR",
        "set_typed": "ASN_OCTET_STR",
        "is_string": True,
    },
}

def oid_dotted_to_array(oid: str) -> List[int]:
    parts = [p.strip() for p in oid.split(".") if p.strip()]
    if not all(re.fullmatch(r"\d+", p) for p in parts):
        raise ValueError(f"OID must be numeric dotted: {oid}")
    return [int(p) for p in parts]

def _get(row: Dict[str, str], key: str) -> Optional[str]:
    v = row.get(key)
    if v is None:
        return None
    v = v.strip()
    return v if v else None

# ---------------- CSV models ----------------

@dataclass
class ScalarDef:
    name: str
    oid: str
    asn: str
    access: str  # RO/RW
    get_fn: str
    set_fn: Optional[str]
    cache_key: str
    # wrapper metadata
    src_struct: Optional[str] = None
    src_reader: Optional[str] = None
    src_field: Optional[str] = None
    src_field_type: Optional[str] = None
    src_max_len: Optional[str] = None
    err_type: Optional[str] = None
    err_ok: Optional[str] = None

@dataclass
class TableDef:
    table_name: str
    entry_name: str
    base_oid: str
    index_name: str
    index_type: str
    index_min: int
    index_max: int
    min_column: int
    max_column: int
    access: str  # RO/RW (RO supported here)

@dataclass
class TableColDef:
    table_name: str
    col: int
    name: str
    asn: str
    get_fn: str
    # wrapper metadata
    src_struct: Optional[str] = None
    src_reader: Optional[str] = None
    src_field: Optional[str] = None
    src_field_type: Optional[str] = None
    src_max_len: Optional[str] = None
    err_type: Optional[str] = None
    err_ok: Optional[str] = None

# ---------------- Read CSVs ----------------

def read_oids_csv(path: Path) -> List[ScalarDef]:
    out: List[ScalarDef] = []
    with path.open(newline="") as f:
        r = csv.DictReader(f)
        required = ["name", "oid", "asn", "access", "get_fn", "set_fn", "cache_key"]
        for k in required:
            if k not in (r.fieldnames or []):
                raise ValueError(f"Missing oids.csv column: {k}")

        for row in r:
            sd = ScalarDef(
                name=row["name"].strip(),
                oid=row["oid"].strip(),
                asn=row["asn"].strip(),
                access=row["access"].strip().upper(),
                get_fn=row["get_fn"].strip(),
                set_fn=row["set_fn"].strip() or None,
                cache_key=row["cache_key"].strip() or f"{row['name'].strip()}_cache",
                src_struct=_get(row, "src_struct"),
                src_reader=_get(row, "src_reader"),
                src_field=_get(row, "src_field"),
                src_field_type=_get(row, "src_field_type"),
                src_max_len=_get(row, "src_max_len"),
                err_type=_get(row, "err_type"),
                err_ok=_get(row, "err_ok"),
            )

            if sd.asn not in ASN_INFO:
                raise ValueError(f"Unsupported ASN type {sd.asn} for {sd.name}")
            if sd.access not in ("RO", "RW"):
                raise ValueError(f"access must be RO or RW for {sd.name}")
            if sd.access == "RW" and not sd.set_fn:
                raise ValueError(f"RW requires set_fn for {sd.name}")

            oid_dotted_to_array(sd.oid)
            out.append(sd)
    return out

def read_tables_csv(path: Path) -> List[TableDef]:
    out: List[TableDef] = []
    with path.open(newline="") as f:
        r = csv.DictReader(f)
        required = ["table_name","entry_name","base_oid","index_name","index_type",
                    "index_min","index_max","min_column","max_column","access"]
        for k in required:
            if k not in (r.fieldnames or []):
                raise ValueError(f"Missing tables.csv column: {k}")

        for row in r:
            td = TableDef(
                table_name=row["table_name"].strip(),
                entry_name=row["entry_name"].strip(),
                base_oid=row["base_oid"].strip(),
                index_name=row["index_name"].strip(),
                index_type=row["index_type"].strip(),
                index_min=int(row["index_min"].strip()),
                index_max=int(row["index_max"].strip()),
                min_column=int(row["min_column"].strip()),
                max_column=int(row["max_column"].strip()),
                access=row["access"].strip().upper(),
            )
            oid_dotted_to_array(td.base_oid)
            if td.index_type not in ("ASN_INTEGER",):
                raise ValueError(f"Only ASN_INTEGER index supported currently: {td.table_name}")
            if td.access not in ("RO","RW"):
                raise ValueError(f"Table access must be RO/RW: {td.table_name}")
            out.append(td)
    return out

def read_table_columns_csv(path: Path) -> List[TableColDef]:
    out: List[TableColDef] = []
    with path.open(newline="") as f:
        r = csv.DictReader(f)
        required = ["table_name","col","name","asn","get_fn",
                    "src_struct","src_reader","src_field","src_field_type","src_max_len","err_type","err_ok"]
        for k in required:
            if k not in (r.fieldnames or []):
                raise ValueError(f"Missing table_columns.csv column: {k}")

        for row in r:
            cd = TableColDef(
                table_name=row["table_name"].strip(),
                col=int(row["col"].strip()),
                name=row["name"].strip(),
                asn=row["asn"].strip(),
                get_fn=row["get_fn"].strip(),
                src_struct=_get(row, "src_struct"),
                src_reader=_get(row, "src_reader"),
                src_field=_get(row, "src_field"),
                src_field_type=_get(row, "src_field_type"),
                src_max_len=_get(row, "src_max_len"),
                err_type=_get(row, "err_type"),
                err_ok=_get(row, "err_ok"),
            )
            if cd.asn not in ASN_INFO:
                raise ValueError(f"Unsupported ASN type {cd.asn} for {cd.name}")
            out.append(cd)
    return out

# ---------------- Code emit helpers ----------------

def cpp_header(module: str) -> str:
    return f"""\
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

#include <string>
#include <cstring>
#include <cstdint>

/* Auto-generated file. Do not edit by hand.
 * Module: {module}
 */

"""

def wrapper_h_header(module: str) -> str:
    return f"""\
#pragma once
#include <string>
#include <cstdint>

/* Auto-generated wrappers for {module}. */

"""

def wrappers_cpp_header(module: str, user_includes: List[str], header_name: str) -> str:
    incs = "\n".join(f'#include <{h}>' for h in user_includes)
    return f"""\
#include <string>
#include <cstring>
#include <cstdint>

#include "{header_name}"
{incs}

/* Auto-generated. Do not edit by hand. */

"""

def _emit_get_error_block(err_snmp: str = "SNMP_ERR_GENERR") -> str:
    return f"""\
            if (ret != 0) {{
                netsnmp_set_request_error(reqinfo, requests, {err_snmp});
                return SNMP_ERR_NOERROR;
            }}
"""


def table_wrapper_impl(c: TableColDef) -> str:
    """
    Generates wrapper implementation for a TABLE column.
    Signature form:
        int get_fn(buc_e idx, std::string &out)
        int get_fn(buc_e idx, u_long *out)
    """

    st = c.src_struct
    reader = c.src_reader
    field = c.src_field
    err_t = c.err_type
    err_ok = c.err_ok
    ft = c.src_field_type

    if ft == "string":
        max_len = c.src_max_len or "MAX_LEN"
        return f"""\
int {c.get_fn}(buc_e idx, std::string &out)
{{
    {st} fcfg{{}};
    {err_t} err_read = {reader}(fcfg, idx, false);
    if (err_read != {err_ok}) {{
        return -1;
    }}
{_string_field_copy(f"fcfg.{field}", max_len).rstrip()}
    return 0;
}}
"""
    elif ft in ("int32", "uint32", "int64", "uint64"):
        return f"""\
int {c.get_fn}(buc_e idx, u_long *out)
{{
    if (!out) return -1;
    {st} fcfg{{}};
    {err_t} err_read = {reader}(fcfg, idx, false);
    if (err_read != {err_ok}) {{
        return -1;
    }}
    *out = (u_long)fcfg.{field};
    return 0;
}}
"""
    else:
        raise ValueError(f"Unsupported src_field_type={ft} for table column {c.name}")

def scalar_wrapper_impl(d: ScalarDef) -> str:
    """
    Generates wrapper implementation for a SCALAR OID.
    Signature form:
        int get_fn(std::string &out)
        int get_fn(u_long *out)
    """

    st = d.src_struct
    reader = d.src_reader
    field = d.src_field
    err_t = d.err_type
    err_ok = d.err_ok
    ft = d.src_field_type

    if ft == "string":
        max_len = d.src_max_len or "MAX_LEN"
        return f"""\
int {d.get_fn}(std::string &out)
{{
    {st} fcfg{{}};
    {err_t} err_read = {reader}(fcfg);
    if (err_read != {err_ok}) {{
        return -1;
    }}
{_string_field_copy(f"fcfg.{field}", max_len).rstrip()}
    return 0;
}}
"""
    elif ft in ("int32", "uint32", "int64", "uint64"):
        return f"""\
int {d.get_fn}(u_long *out)
{{
    if (!out) return -1;
    {st} fcfg{{}};
    {err_t} err_read = {reader}(fcfg);
    if (err_read != {err_ok}) {{
        return -1;
    }}
    *out = (u_long)fcfg.{field};
    return 0;
}}
"""
    else:
        raise ValueError(f"Unsupported src_field_type={ft} for scalar {d.name}")


def _string_field_copy(src_expr: str, max_len: Optional[str]) -> str:
    if not max_len:
        return f"    out = {src_expr};\n"
    return f"""\
    size_t n = strnlen({src_expr}, {max_len});
    out.assign({src_expr}, n);
"""

def wants_scalar_wrapper(d: ScalarDef) -> bool:
    return bool(d.src_struct and d.src_reader and d.src_field and d.src_field_type and d.err_type and d.err_ok)

def wants_table_wrapper(c: TableColDef) -> bool:
    return bool(c.src_struct and c.src_reader and c.src_field and c.src_field_type and c.err_type and c.err_ok)

# ---------------- Scalar generation ----------------

def emit_scalar_handler_decl(defs: List[ScalarDef]) -> str:
    return "\n".join(
        f"int handle_{d.name}(netsnmp_mib_handler *, netsnmp_handler_registration *, "
        f"netsnmp_agent_request_info *, netsnmp_request_info *);"
        for d in defs
    ) + "\n\n"

def emit_scalar_oid_arrays(defs: List[ScalarDef]) -> str:
    lines = []
    for d in defs:
        arr = ", ".join(str(n) for n in oid_dotted_to_array(d.oid))
        lines.append(f"static const oid {d.name}_oid[] = {{ {arr} }};")
    return "\n".join(lines) + "\n\n"

def emit_scalar_init(module: str, defs: List[ScalarDef]) -> str:
    lines: List[str] = []
    lines.append(f"void init_{module}_scalars(void)")
    lines.append("{")
    for d in defs:
        perms = "HANDLER_CAN_RONLY" if d.access == "RO" else "HANDLER_CAN_RWRITE"
        lines.append("    netsnmp_register_scalar(")
        lines.append("        netsnmp_create_handler_registration(")
        lines.append(f"            \"{d.name}\", handle_{d.name},")
        lines.append(f"            {d.name}_oid, OID_LENGTH({d.name}_oid),")
        lines.append(f"            {perms}")
        lines.append("        )")
        lines.append("    );")
        lines.append("")
    lines.append("}")
    return "\n".join(lines) + "\n\n"

def emit_scalar_handler(d: ScalarDef, get_error_snmp: str = "SNMP_ERR_GENERR") -> str:
    info = ASN_INFO[d.asn]
    asn = d.asn

    if info["is_string"]:
        asn = d.asn
        max_len = d.src_max_len or None  # only if you want to enforce it here

        get_body = f"""\
            case MODE_GET: 
            case MODE_GETNEXT: {{
                std::string s;
                int ret = {d.get_fn}(s);
    {_emit_get_error_block(get_error_snmp).rstrip()}
                snmp_set_var_typed_value(
                    requests->requestvb,
                    {asn},
                    (const u_char*)s.data(),
                    s.size()
                );
                break;
            }}
    """
        set_cases = ""
        if d.access == "RW":
            # optional length check if you have a numeric macro like MAX_LEN
            length_check = ""
            if d.src_max_len:
                length_check = f"""
                if (requests->requestvb->val_len > {d.src_max_len}) {{
                    netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_WRONGLENGTH);
                }}
    """

            set_cases = f"""
            case MODE_SET_RESERVE1: {{
                int ret = netsnmp_check_vb_type(requests->requestvb, {info["check"]});
                if (ret != SNMP_ERR_NOERROR) {{
                    netsnmp_set_request_error(reqinfo, requests, ret);
                    break;
                }}
    {length_check.rstrip()}
                break;
            }}

            case MODE_SET_RESERVE2: {{
                // cache old value for UNDO
                std::string oldv;
                int ret = {d.get_fn}(oldv);
                (void)ret;

                auto *cache = new(std::nothrow) std::string(oldv);
                if (!cache) {{
                    netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_RESOURCEUNAVAILABLE);
                    return SNMP_ERR_NOERROR;
                }}

                netsnmp_request_add_list_data(
                    requests,
                    netsnmp_create_data_list("{d.cache_key or (d.name + '_cache')}", cache,
                        [](void *p) {{ delete (std::string*)p; }})
                );
                break;
            }}

            case MODE_SET_FREE:
                break;

            case MODE_SET_ACTION: {{
                const u_char *p = (const u_char*)requests->requestvb->val.string;
                size_t n = requests->requestvb->val_len;
                std::string v((const char*)p, n);

                int ret = {d.set_fn}(v);
                if (ret == SNMP_ERR_BADVALUE) {{
                    netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_BADVALUE);
                }}
                break;
            }}

            case MODE_SET_COMMIT:
                break;

            case MODE_SET_UNDO: {{
                auto *oldp = (std::string*)netsnmp_request_get_list_data(requests, "{d.cache_key or (d.name + '_cache')}");
                if (!oldp) {{
                    netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_UNDOFAILED);
                    break;
                }}
                int ret = {d.set_fn}(*oldp);
                if (ret == SNMP_ERR_BADVALUE) {{
                    netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_UNDOFAILED);
                }}
                break;
            }}
    """
    else:
        ctype = info["ctype"]
        get_body = f"""\
        case MODE_GET: 
        case MODE_GETNEXT: {{
            {ctype} value = 0;
            int ret = {d.get_fn}(&value);
{_emit_get_error_block(get_error_snmp).rstrip()}
            snmp_set_var_typed_value(
                requests->requestvb,
                {info["set_typed"]},
                {info["value_ptr"]},
                {info["vb_value_len"]}
            );
            break;
        }}
"""
        set_cases = ""
        if d.access == "RW":
            set_cases = f"""
        case MODE_SET_RESERVE1: {{
            int ret = netsnmp_check_vb_type(requests->requestvb, {info["check"]});
            if (ret != SNMP_ERR_NOERROR) {{
                netsnmp_set_request_error(reqinfo, requests, ret);
            }}
            break;
        }}

        case MODE_SET_RESERVE2: {{
            {ctype} value = 0;
            int ret = {d.get_fn}(&value);
            (void)ret;
            auto *cache = ({ctype}*)netsnmp_memdup(&value, sizeof(value));
            if (cache == NULL) {{
                netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_RESOURCEUNAVAILABLE);
                return SNMP_ERR_NOERROR;
            }}
            netsnmp_request_add_list_data(
                requests,
                netsnmp_create_data_list("{d.cache_key}", cache, free)
            );
            break;
        }}

        case MODE_SET_FREE:
            break;

        case MODE_SET_ACTION: {{
            {ctype} value = ({ctype}){info["get_from_vb"]};
            int ret = {d.set_fn}(&value);
            if (ret == SNMP_ERR_BADVALUE) {{
                netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_BADVALUE);
            }}
            break;
        }}

        case MODE_SET_COMMIT:
            break;

        case MODE_SET_UNDO: {{
            {ctype} value = *(({ctype}*)netsnmp_request_get_list_data(requests, "{d.cache_key}"));
            int ret = {d.set_fn}(&value);
            if (ret == SNMP_ERR_BADVALUE) {{
                netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_UNDOFAILED);
            }}
            break;
        }}
"""

    lines: List[str] = []
    lines.append(f"int handle_{d.name}(netsnmp_mib_handler *handler,")
    lines.append(f"             netsnmp_handler_registration *reginfo,")
    lines.append(f"             netsnmp_agent_request_info   *reqinfo,")
    lines.append(f"             netsnmp_request_info         *requests)")
    lines.append("{")
    lines.append("    (void)handler; (void)reginfo;")
    lines.append("    switch(reqinfo->mode) {")
    lines.append(get_body.rstrip())
    if set_cases.strip():
        lines.append(set_cases.rstrip())
    lines.append("")
    lines.append("        default:")
    lines.append(f"            snmp_log(LOG_ERR, \"unknown mode (%d) in handle_{d.name}\\\\n\", reqinfo->mode);")
    lines.append("            return SNMP_ERR_GENERR;")
    lines.append("    }")
    lines.append("")
    lines.append("    return SNMP_ERR_NOERROR;")
    lines.append("}")
    return "\n".join(lines) + "\n\n"

# ---------------- Table generation (iterator) ----------------

def emit_table_init_decl(tables: List[TableDef]) -> str:
    return "\n".join(f"static void init_{t.table_name}(void);" for t in tables) + "\n\n"

def emit_table_code(table: TableDef, cols: List[TableColDef]) -> str:
    # Row context
    ctx_name = f"{table.table_name}_row_ctx"
    base_arr = ", ".join(str(n) for n in oid_dotted_to_array(table.base_oid))

    # switch cases for columns
    col_by_num = {c.col: c for c in cols}
    switch_cases: List[str] = []
    for col in range(table.min_column, table.max_column + 1):
        c = col_by_num.get(col)
        if not c:
            continue

        info = ASN_INFO[c.asn]
        if info["is_string"]:
            switch_cases.append(f"""\
            case {col}: {{ /* {c.name} */
                std::string s;
                int ret = {c.get_fn}(idx, s);
                if (ret != 0) {{
                    netsnmp_set_request_error(reqinfo, req, SNMP_ERR_GENERR);
                    break;
                }}
                snmp_set_var_typed_value(req->requestvb, ASN_OCTET_STR,
                                         (const u_char*)s.data(), s.size());
                break;
            }}
""")
        else:
            switch_cases.append(f"""\
            case {col}: {{ /* {c.name} */
                u_long v = 0;
                int ret = {c.get_fn}(idx, &v);
                if (ret != 0) {{
                    netsnmp_set_request_error(reqinfo, req, SNMP_ERR_GENERR);
                    break;
                }}
                snmp_set_var_typed_value(req->requestvb, {c.asn},
                                         (u_char*)&v, sizeof(v));
                break;
            }}
""")

    switch_body = "\n".join(switch_cases) if switch_cases else "            /* no columns */\n"

    return f"""\
/* -------- Table: {table.table_name} -------- */

struct {ctx_name} {{
    int idx;
}};

static netsnmp_variable_list *
{table.table_name}_get_first(void **loop_ctx, void **data_ctx,
                            netsnmp_variable_list *index, netsnmp_iterator_info *iinfo)
{{
    (void)iinfo;
    auto *ctx = new {ctx_name}{{ {table.index_min} }};
    *loop_ctx = ctx;
    *data_ctx = ctx;

    snmp_set_var_typed_integer(index, {table.index_type}, ctx->idx);
    return index;
}}

static netsnmp_variable_list *
{table.table_name}_get_next(void **loop_ctx, void **data_ctx,
                           netsnmp_variable_list *index, netsnmp_iterator_info *iinfo)
{{
    (void)iinfo;
    auto *ctx = ({ctx_name}*)(*loop_ctx);
    if (!ctx) return nullptr;

    ctx->idx++;

    if (ctx->idx > {table.index_max}) {{
        delete ctx;
        *loop_ctx = nullptr;
        *data_ctx = nullptr;
        return nullptr;
    }}

    *data_ctx = ctx;
    snmp_set_var_typed_integer(index, {table.index_type}, ctx->idx);
    return index;
}}

static int
{table.table_name}_handler(netsnmp_mib_handler *handler,
                           netsnmp_handler_registration *reginfo,
                           netsnmp_agent_request_info *reqinfo,
                           netsnmp_request_info *requests)
{{
    (void)handler; (void)reginfo;

    for (auto *req = requests; req; req = req->next) {{
        auto *tinfo = netsnmp_extract_table_info(req);
        auto *row = ({ctx_name}*)netsnmp_extract_iterator_context(req);

        if (!tinfo || !row) {{
            netsnmp_set_request_error(reqinfo, req, SNMP_ERR_GENERR);
            continue;
        }}

        const int idx = row->idx;
        const int col = tinfo->colnum;

        if (reqinfo->mode != MODE_GET) {{
            netsnmp_set_request_error(reqinfo, req, SNMP_ERR_GENERR);
            continue;
        }}

        switch (col) {{
{switch_body.rstrip()}
            default:
                netsnmp_set_request_error(reqinfo, req, SNMP_NOSUCHOBJECT);
                break;
        }}
    }}

    return SNMP_ERR_NOERROR;
}}

static void init_{table.table_name}(void)
{{
    static oid base_oid[] = {{ {base_arr} }};

    netsnmp_handler_registration *reg =
        netsnmp_create_handler_registration("{table.table_name}",
                                            {table.table_name}_handler,
                                            base_oid, OID_LENGTH(base_oid),
                                            HANDLER_CAN_RONLY);

    auto *table_info =
        (netsnmp_table_registration_info*)calloc(1, sizeof(*table_info));
    auto *iinfo =
        (netsnmp_iterator_info*)calloc(1, sizeof(*iinfo));

    netsnmp_table_helper_add_indexes(table_info, {table.index_type}, 0);

    table_info->min_column = {table.min_column};
    table_info->max_column = {table.max_column};

    iinfo->get_first_data_point = {table.table_name}_get_first;
    iinfo->get_next_data_point  = {table.table_name}_get_next;
    iinfo->table_reginfo        = table_info;

    netsnmp_register_table_iterator(reg, iinfo);
}}

"""

# ---------------- Wrapper generation (scalars + table columns) ----------------

def emit_wrappers_h(module: str, scalars: List[ScalarDef], tcols: List[TableColDef]) -> str:
    lines: List[str] = [wrapper_h_header(module)]

    # scalar wrappers
    for d in scalars:
        if not wants_scalar_wrapper(d):
            continue
        ft = d.src_field_type
        if ft == "string":
            lines.append(f"int {d.get_fn}(std::string &out);")
        else:
            lines.append(f"int {d.get_fn}(u_long *out);")

    if any(wants_scalar_wrapper(d) for d in scalars):
        lines.append("")

    # table wrappers
    for c in tcols:
        if not wants_table_wrapper(c):
            continue
        ft = c.src_field_type
        if ft == "string":
            lines.append(f"int {c.get_fn}(buc_e idx, std::string &out);")
        else:
            lines.append(f"int {c.get_fn}(buc_e idx, u_long *out);")

    lines.append("")
    return "\n".join(lines)

def emit_table_wrappers_cpp(module: str,
                            table: TableDef,
                            tcols_for_table: List[TableColDef],
                            header_name: str) -> str:
    user_includes = ["lib_cfg_cntrlr_factory.h", "lib_json.h", "lib_error.h"]
    out: List[str] = [wrappers_cpp_preamble(module, header_name, user_includes)]
    out.append(f"/* Wrappers for table: {table.table_name} */\n\n")

    for c in tcols_for_table:
        if not wants_table_wrapper(c):
            continue
        out.append(table_wrapper_impl(c))
        out.append("\n\n")
    return "".join(out)

def emit_scalar_wrappers_cpp(module: str, scalars: List[ScalarDef], header_name: str) -> str:
    user_includes = ["lib_cfg_cntrlr_factory.h", "lib_json.h", "lib_error.h"]
    out: List[str] = [wrappers_cpp_preamble(module, header_name, user_includes)]

    for d in scalars:
        if not wants_scalar_wrapper(d):
            continue
        out.append(scalar_wrapper_impl(d))
        out.append("\n\n")
    return "".join(out)

def wrappers_cpp_preamble(module: str, header_name: str, user_includes: List[str]) -> str:
    incs = "\n".join(f'#include <{h}>' for h in user_includes)
    return f"""\
#include <string>
#include <cstring>
#include <cstdint>

#include "{header_name}"
{incs}

/* Auto-generated. Do not edit by hand. */

"""

def emit_wrappers_cpp(module: str,
                      scalars: List[ScalarDef],
                      tcols: List[TableColDef],
                      header_name: str) -> str:
    # Update these to match your project
    user_includes = [
        "cfg_cntrlr_factory.h",
        "json_readers.h",
    ]

    out: List[str] = [wrappers_cpp_preamble(module, header_name, user_includes)]

    # Scalar wrapper impls: reader(fcfg)
    for d in scalars:
        if not wants_scalar_wrapper(d):
            continue
        st, reader, field, err_t, err_ok, ft = d.src_struct, d.src_reader, d.src_field, d.err_type, d.err_ok, d.src_field_type
        if ft == "string":
            max_len = d.src_max_len or "MAX_LEN"
            out.append(f"""\
int {d.get_fn}(std::string &out)
{{
    {st} fcfg{{}};
    {err_t} err_read = {reader}(fcfg);
    if (err_read != {err_ok}) {{
        return -1;
    }}
{_string_field_copy(f"fcfg.{field}", max_len).rstrip()}
    return 0;
}}

""")
        else:
            out.append(f"""\
int {d.get_fn}(u_long *out)
{{
    if (!out) return -1;
    {st} fcfg{{}};
    {err_t} err_read = {reader}(fcfg);
    if (err_read != {err_ok}) {{
        return -1;
    }}
    *out = (u_long)fcfg.{field};
    return 0;
}}

""")

    # Table wrapper impls: reader(idx, fcfg)
    for c in tcols:
        if not wants_table_wrapper(c):
            continue
        st, reader, field, err_t, err_ok, ft = c.src_struct, c.src_reader, c.src_field, c.err_type, c.err_ok, c.src_field_type
        if ft == "string":
            max_len = c.src_max_len or "MAX_LEN"
            out.append(f"""\
int {c.get_fn}(buc_e idx, std::string &out)
{{
    {st} fcfg{{}};
    {err_t} err_read = {reader}(fcfg, idx, false);
    if (err_read != {err_ok}) {{
        return -1;
    }}
{_string_field_copy(f"fcfg.{field}", max_len).rstrip()}
    return 0;
}}

""")
        else:
            out.append(f"""\
int {c.get_fn}(buc_e idx, u_long *out)
{{
    if (!out) return -1;
    {st} fcfg{{}};
    {err_t} err_read = {reader}(fcfg, idx, false);
    if (err_read != {err_ok}) {{
        return -1;
    }}
    *out = (u_long)fcfg.{field};
    return 0;
}}

""")

    return "".join(out)

def generate_module_glue_cpp(module: str) -> str:
    return cpp_header(module) + f"""\
/* Glue file: calls scalars + tables init */

void init_{module}_scalars(void);
void init_{module}_tables(void);

void init_{module}(void)
{{
    init_{module}_scalars();
    init_{module}_tables();
}}
"""


def generate_tables_cpp(module: str, tables: List[TableDef], tcols: List[TableColDef]) -> str:
    out: List[str] = []
    out.append(cpp_header(module))
    out.append(f'#include "{module}.h"\n\n')

    # group columns by table
    cols_by_table: DefaultDict[str, List[TableColDef]] = defaultdict(list)
    for c in tcols:
        cols_by_table[c.table_name].append(c)
    for tname in cols_by_table:
        cols_by_table[tname].sort(key=lambda x: x.col)

    # emit all table code
    for t in tables:
        out.append(emit_table_code(t, cols_by_table.get(t.table_name, [])))

    # Exported init function for tables
    out.append(f"""\
void init_{module}_tables(void)
{{
""")
    for t in tables:
        out.append(f"    init_{t.table_name}();\n")
    out.append("}\n")
    return "".join(out)


def generate_scalars_cpp(module: str, scalars: List[ScalarDef]) -> str:
    out: List[str] = []
    out.append(cpp_header(module))
    out.append(f'#include "{module}.h"\n\n')

    out.append(emit_scalar_handler_decl(scalars))
    out.append(emit_scalar_oid_arrays(scalars))
    out.append(emit_scalar_init(module, scalars))
    for d in scalars:
        out.append(emit_scalar_handler(d, get_error_snmp="SNMP_ERR_GENERR"))

    return "".join(out)


# ---------------- Main agent file generation ----------------

def generate_agent_cpp(module: str,
                       scalars: List[ScalarDef],
                       tables: List[TableDef],
                       tcols: List[TableColDef]) -> str:
    out: List[str] = []
    out.append(cpp_header(module))
    out.append(f'#include "{module}_wrappers.h"\n\n')

    # Scalars
    out.append(emit_scalar_handler_decl(scalars))
    out.append(emit_scalar_oid_arrays(scalars))
    out.append(emit_scalar_init(module, scalars))
    for d in scalars:
        out.append(emit_scalar_handler(d, get_error_snmp="SNMP_ERR_GENERR"))

    # Tables
    out.append(emit_table_init_decl(tables))

    # group columns by table
    cols_by_table: DefaultDict[str, List[TableColDef]] = defaultdict(list)
    for c in tcols:
        cols_by_table[c.table_name].append(c)
    for tname in cols_by_table:
        cols_by_table[tname].sort(key=lambda x: x.col)

    for t in tables:
        out.append(emit_table_code(t, cols_by_table.get(t.table_name, [])))

    # Final module init that calls scalars + all tables
    out.append(f"""\
void init_{module}(void)
{{
    init_{module}_scalars();
""")
    for t in tables:
        out.append(f"    init_{t.table_name}();\n")
    out.append("}\n")

    return "".join(out)

# ---------------- CLI ----------------

def main():
    if len(sys.argv) != 6:
        print("Usage:", file=sys.stderr)
        print("  mib_autogen.py <module> <oids.csv> <tables.csv> <table_columns.csv> <out_dir>", file=sys.stderr)
        sys.exit(2)

    module = sys.argv[1].strip()
    oids_csv = Path(sys.argv[2])
    tables_csv = Path(sys.argv[3])
    tcols_csv = Path(sys.argv[4])
    out_dir = Path(sys.argv[5])
    out_dir.mkdir(parents=True, exist_ok=True)

    scalars = read_oids_csv(oids_csv)
    tables = read_tables_csv(tables_csv)
    tcols = read_table_columns_csv(tcols_csv)

    # Validate table columns refer to known tables
    table_names = {t.table_name for t in tables}
    for c in tcols:
        if c.table_name not in table_names:
            raise ValueError(f"table_columns.csv references unknown table: {c.table_name}")

    # Write wrappers
    # Write wrappers header
    wrappers_h_name = f"{module}.h"
    wrappers_h = emit_wrappers_h(module, scalars, tcols)
    (out_dir / wrappers_h_name).write_text(wrappers_h)

    # Group table columns by table
    cols_by_table: DefaultDict[str, List[TableColDef]] = defaultdict(list)
    for c in tcols:
        cols_by_table[c.table_name].append(c)
    for tname in cols_by_table:
        cols_by_table[tname].sort(key=lambda x: x.col)

    # Write scalar wrappers (one file)
    scal_wrappers_cpp = emit_scalar_wrappers_cpp(module, scalars, wrappers_h_name)
    scal_wrappers_path = out_dir / f"{module}_scalars.cpp"
    scal_wrappers_path.write_text(scal_wrappers_cpp)

    # Write one wrapper file per table
    written_tables = 0
    for t in tables:
        tcols_for_table = cols_by_table.get(t.table_name, [])
        table_cpp = emit_table_wrappers_cpp(module, t, tcols_for_table, wrappers_h_name)
        table_path = out_dir / f"{module}_{t.table_name}.cpp"
        table_path.write_text(table_cpp)
        written_tables += 1

    print(f"Wrote: {out_dir / wrappers_h_name}")
    print(f"Wrote: {scal_wrappers_path}")
    print(f"Wrote: {written_tables} table wrapper files")


    agent_cpp = generate_agent_cpp(module, scalars, tables, tcols)
    agent_path = out_dir / f"{module}_agent.cpp"
    agent_path.write_text(agent_cpp)
    print(f"Wrote: {agent_path}")

if __name__ == "__main__":
    main()
