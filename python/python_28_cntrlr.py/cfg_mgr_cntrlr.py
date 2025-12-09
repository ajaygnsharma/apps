#-------------------------------------------------------------------------------
#
# cfg_mgr_cntrlr.py
#
# Configuration manager for Controller box. It uses caching because there
# is chance that the configuration is changed every so often. So check
# if nothing is changed, then dont write. Save flash write cycles.
#
#-------------------------------------------------------------------------------
import os
import socket
import json
import configparser
from dataclasses import dataclass
from dataclasses import asdict
from lib_cfg import ibuc_log
from lib_cfg import unquote

CONFIG_PATH              = "ibuc_cntrlr.conf"
SOCKET_PATH              = "/tmp/cntrlr_cfgd.sock"

config_cache             = []
config_mtime             = 0

SWITCHING_TYPE_AUTOMATIC = 0
TEN_MHZ_EXTERNAL         = 0

#-------------------------------------------------------------------------------
#
# class CntrlrData
#
#-------------------------------------------------------------------------------
@dataclass
class CntrlrData:
    version: int
    user_title: str
    simulate: int
    suppress: int
    major_mask: int
    minor_mask: int
    suppress_mask: int
    stat_log_time: int
    time_zone_offset: int
    switching_mode: int
    wg_sw1_pos1: int
    wg_sw1_pos2: int
    wg_sw2_pos1: int
    wg_sw2_pos2: int
    ifl_A_to_spare: int
    ifl_B_to_spare: int
    attenuation_A_to_Spare_mBm: int
    attenuation_B_to_Spare_mBm: int
    tenMHz_source: int
    tenMHz_trim_A: int
    tenMHz_trim_B: int
    ip_addr: str
    mask: str
    gateway: str
    firmware: str

#-------------------------------------------------------------------------------
#
# check_and_reload_config . We are not allowing external changes to config file.
# No need to monitor it. Keep it simple.
#
#-------------------------------------------------------------------------------
'''def check_and_reload_config():
    global config_cache, config_mtime

    try:
        current_mtime = os.path.getmtime(CONFIG_PATH)
        if current_mtime != config_mtime:
            print("Config file modified externally, reloading...")
            new_cfg = load_config(CONFIG_PATH)
            if new_cfg:
                config_cache = new_cfg
                config_mtime = current_mtime
    except FileNotFoundError:
        print("Config file not found:", CONFIG_PATH)
'''

#-------------------------------------------------------------------------------
#
# handle_request
#
#-------------------------------------------------------------------------------
def handle_request(data):
    try:
        #check_and_reload_config()

        global config_cache;

        req = json.loads(data.decode())
        cmd = req.get("cmd")
        if cmd == "get_cntrlr_cfg":
            return json.dumps({"status": "ok", "settings": asdict(config_cache)}).encode()
        elif cmd == "set_cntrlr_cfg":
            cfg = req["settings"]

            # Compare against current config before updating
            incoming_cfg_obj = CntrlrData(
                version=int(cfg.get("version", config_cache.version)),
                user_title=unquote(cfg.get("user_title", config_cache.user_title)),
                simulate=int(cfg.get("simulate", config_cache.simulate)),
                suppress=int(cfg.get("suppress", config_cache.suppress)),
                major_mask=int(cfg.get("major_mask", config_cache.major_mask)),
                minor_mask=int(cfg.get("minor_mask", config_cache.minor_mask)),
                suppress_mask=int(cfg.get("suppress_mask", config_cache.suppress_mask)),
                stat_log_time=int(cfg.get("stat_log_time", config_cache.stat_log_time)),
                time_zone_offset=int(cfg.get("time_zone_offset", config_cache.time_zone_offset)),
                switching_mode=int(cfg.get("switching_mode", config_cache.switching_mode)),
                wg_sw1_pos1=int(cfg.get("wg_sw1_pos1", config_cache.wg_sw1_pos1)),
                wg_sw1_pos2=int(cfg.get("wg_sw1_pos2", config_cache.wg_sw1_pos2)),
                wg_sw2_pos1=int(cfg.get("wg_sw2_pos1", config_cache.wg_sw2_pos1)),
                wg_sw2_pos2=int(cfg.get("wg_sw2_pos2", config_cache.wg_sw2_pos2)),
                ifl_A_to_spare=int(cfg.get("ifl_A_to_spare", config_cache.ifl_A_to_spare)),
                ifl_B_to_spare=int(cfg.get("ifl_B_to_spare", config_cache.ifl_B_to_spare)),
                attenuation_A_to_Spare_mBm=int(cfg.get("attenuation_A_to_Spare_mBm", config_cache.attenuation_A_to_Spare_mBm)),
                attenuation_B_to_Spare_mBm=int(cfg.get("attenuation_B_to_Spare_mBm", config_cache.attenuation_B_to_Spare_mBm)),
                tenMHz_source=int(cfg.get("tenMHz_source", config_cache.tenMHz_source)),
                tenMHz_trim_A=int(cfg.get("tenMHz_trim_A", config_cache.tenMHz_trim_A)),
                tenMHz_trim_B=int(cfg.get("tenMHz_trim_B", config_cache.tenMHz_trim_B)),
                ip_addr=unquote(cfg.get("ip_addr", config_cache.ip_addr)),
                mask=unquote(cfg.get("mask", config_cache.mask)),
                gateway=unquote(cfg.get("gateway", config_cache.gateway)),
                firmware=unquote(cfg.get("firmware", config_cache.firmware)),
            )

            ''' Write to file only if the data is changed '''
            if incoming_cfg_obj == config_cache:
                return json.dumps({"status": "ok", "note": "no change"}).encode()

            config_cache = incoming_cfg_obj;
            save_config(CONFIG_PATH, asdict(incoming_cfg_obj));
            return json.dumps({"status": "ok"}).encode()
        elif cmd == "reset_cntrlr_cfg":
            save_default()
        else:
            return json.dumps({"status": "error", "error": "Unknown command"}).encode()
    except Exception as e:
        return json.dumps({"status": "error", "error": str(e)}).encode()


#-------------------------------------------------------------------------------
#
# save_config
#
#-------------------------------------------------------------------------------
def save_config(config_path, config_data):
    config = configparser.ConfigParser()
    config.optionxform = str
    config["settings"] = {}
    main = config["settings"]

    # Flatten directly into INI-style keys
    for key, value in config_data.items():
        if isinstance(value, (int, float, str)):
            main[key] = str(value)
        else:
            raise ValueError(f"Unsupported config value type for key '{key}': {type(value)}")

    # Atomic write: write to temp then rename
    tmp_path = config_path + ".tmp"
    with open(tmp_path, "w") as f:
        config.write(f)
    os.rename(tmp_path, config_path)

#-------------------------------------------------------------------------------
#
# Save defaults
#
#-------------------------------------------------------------------------------
def save_default() -> CntrlrData:
    def_cfg = CntrlrData(
        version                     =1,
        user_title                  ="Default",
        simulate                    =0,
        suppress                    =0,
        major_mask                  =65355,  # All alarms are major for sake of simplicity
        minor_mask                  =0,
        suppress_mask               =0,
        stat_log_time               =1,  # in minute
        time_zone_offset            =0,
        switching_mode              =SWITCHING_TYPE_AUTOMATIC,
        wg_sw1_pos1                 =0,
        wg_sw1_pos2                 =1,
        wg_sw2_pos1                 =0,
        wg_sw2_pos2                 =1,
        ifl_A_to_spare              =1,
        ifl_B_to_spare              =0,
        attenuation_A_to_Spare_mBm  =0,
        attenuation_B_to_Spare_mBm  =0,
        tenMHz_source               =TEN_MHZ_EXTERNAL,
        tenMHz_trim_A               =0,
        tenMHz_trim_B               =0,
        ip_addr                     ="10.0.0.21",
        mask                        ="255.0.0.0",
        gateway                     ="10.0.0.1",
        firmware                    ="Undefined",
    )
    save_config(CONFIG_PATH, asdict(def_cfg));
    return def_cfg

#-------------------------------------------------------------------------------
#
# load_config
#
#-------------------------------------------------------------------------------
def load_config(file_path, fw_ver) -> CntrlrData:
    try:
        # reads file content
        with open(file_path, 'r') as f:
            file_content = f.read()

        # checks if content has a section header. If not it adds '[setting]'
        if not file_content.strip().startswith('['):
            file_content = '[settings]\n' + file_content

        config = configparser.ConfigParser();
        config.optionxform = str
        config.read_string(file_content);

        cfg = config["settings"];
        cfg_obj = CntrlrData(
            version=int(cfg.get("version", 0)),
            user_title=unquote(cfg.get("user_title", "Default")),
            simulate=int(cfg.get("simulate", 0)),
            suppress=int(cfg.get("suppress", 0)),
            major_mask=int(cfg.get("major_mask", 0)),
            minor_mask=int(cfg.get("minor_mask", 0)),
            suppress_mask=int(cfg.get("suppress_mask", 0)),
            stat_log_time=int(cfg.get("stat_log_time", 0)),
            time_zone_offset=int(cfg.get("time_zone_offset", 0)),
            switching_mode=int(cfg.get("switching_mode", 0)),
            wg_sw1_pos1=int(cfg.get("wg_sw1_pos1", 0)),
            wg_sw1_pos2=int(cfg.get("wg_sw1_pos2", 0)),
            wg_sw2_pos1=int(cfg.get("wg_sw2_pos1", 0)),
            wg_sw2_pos2=int(cfg.get("wg_sw2_pos2", 0)),
            ifl_A_to_spare=int(cfg.get("ifl_A_to_spare", 0)),
            ifl_B_to_spare=int(cfg.get("ifl_B_to_spare", 0)),
            attenuation_A_to_Spare_mBm=int(cfg.get("attenuation_A_to_Spare_mBm", 0)),
            attenuation_B_to_Spare_mBm=int(cfg.get("attenuation_B_to_Spare_mBm", 0)),
            tenMHz_source=int(cfg.get("tenmhz_source", 0)),
            tenMHz_trim_A=int(cfg.get("tenMHz_trim_A", 0)),
            tenMHz_trim_B=int(cfg.get("tenMHz_trim_B", 0)),
            ip_addr=unquote(cfg.get("ip_addr", "")),
            mask=unquote(cfg.get("mask", "")),
            gateway=unquote(cfg.get("gateway", "")),
            firmware=unquote(fw_ver),
        )

        return cfg_obj;
    except FileNotFoundError as e:
        ibuc_log(f"{file_path} not found, creating default")
        cfg = save_default()
        return cfg

    except Exception as e:
        ibuc_log(f"Error loading cfg from {file_path}: {e}");
        cfg = save_default()
        return cfg



#-------------------------------------------------------------------------------
#
# Read firmware revision
#
#-------------------------------------------------------------------------------
def read_fw_ver():
    try:
        with open('/etc/issue', 'r') as file:
            contents = file.read()
            return contents
    except FileNotFoundError:
        print("Error: /etc/issue does not exist.")
        return ""
    except PermissionError:
        print("Error: You don't have permission to read /etc/issue")
        return ""
    except Exception as e:
        print(f"An unexpected error occurred: {e}")
        return ""


#-------------------------------------------------------------------------------
#
# run_server
#
#-------------------------------------------------------------------------------
def run_server():
    global config_cache
    global config_mtime

    if os.path.exists(SOCKET_PATH):
        os.remove(SOCKET_PATH)

    sock = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
    sock.bind(SOCKET_PATH)
    print("Configuration daemon listening on", SOCKET_PATH)

    #import pdb; pdb.set_trace()

    fw_ver = read_fw_ver()
    if len(fw_ver) > 0:
        config_cache = load_config(CONFIG_PATH, fw_ver);
    else:
        config_cache = load_config(CONFIG_PATH, "Undefined");

    print(config_cache)

    try:
        config_mtime = os.path.getmtime(CONFIG_PATH)
    except FileNotFoundError:
        config_mtime = 0

    while True:
        data, addr = sock.recvfrom(4096)
        if not addr:
            print("Warning: No client address received. Skipping response.")
            continue
        response = handle_request(data)

        try:
            sock.sendto(response, addr)
        except Exception as e:
            print(f"Failed to send resp: {e}");

#-------------------------------------------------------------------------------
#
# main()
#
#-------------------------------------------------------------------------------
if __name__ == "__main__":
    run_server()
