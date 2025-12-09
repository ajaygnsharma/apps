import configparser
from dataclasses import dataclass

@dataclass
class FactoryCntrlrData:
    serial_number: str
    model_number: str
    private_label: str
    hardware_revision: str
    total_sys_hours: int
    internal_10MHz: int
    supply_voltage_min: int
    supply_voltage_max: int

def load_factory_cntrlr_cfg(file_path: str) -> FactoryCntrlrData:
    try:
        # reads file content
        with open(file_path, 'r') as f:
            file_content = f.read()

        # checks if content has a section header. If not it adds '[setting]'
        if not file_content.strip().startswith('['):
            file_content = '[settings]\n' + file_content

        config = configparser.ConfigParser()
        config.read_string(file_content)

        settings = config["settings"]

        # populate UnitData
        factoryCntrlr_data = FactoryCntrlrData(
            serial_number=settings.get("serial_number"),
            model_number=settings.get("model_number"),
            private_label="",  # not using
            hardware_revision=settings.get("hardware_revision"),
            total_sys_hours=int(settings.get("total_sys_hours")),
            internal_10MHz=int(settings.get("internal_10MHz")),
            supply_voltage_min=int(settings.get("supply_voltage_min")),
            supply_voltage_max=int(settings.get("supply_voltage_max")),
        )
        return factoryCntrlr_data

    except Exception as e:
        print(f"Error loading config from {file_path}: {e}")
        return None


def not_connected(arr: any) -> bool:
    return arr.model_number.strip('"').strip("'") == "Default"

FactoryCntrlrData = load_factory_cntrlr_cfg("cntlr_factory.conf");
if not_connected(FactoryCntrlrData):
    print("Factory Cntrlr Not Connected");
else:
    print(FactoryCntrlrData)
