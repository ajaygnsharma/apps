import configparser
from dataclasses import dataclass

@dataclass
class FactoryIBUCData:
    serial_number: str
    model_number: str
    private_label: str
    hardware_revision: str
    rated_power_mB: int
    rf_range: str
    rf_freq_band: str
    lo_ID: int
    lo_freq: int
    lo_spectral_inversion: str
    if_range: str
    total_sys_hours: int
    internal_10MHz: int
    supply_voltage_min: int
    supply_voltage_max: int


def load_factory_ibuc_cfg(file_path: str) -> FactoryIBUCData:
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
        factoryIBUC_data = FactoryIBUCData(
            serial_number = settings.get("serial_number"),
            model_number = settings.get("model_number"),
            private_label = settings.get("private_label"),
            hardware_revision = settings.get("hardware_revision"),
            rated_power_mB = int(settings.get("rated_power_mB")),
            rf_range = settings.get("rf_range"),
            rf_freq_band = settings.get("rf_freq_band"),
            lo_ID = int(settings.get("lo_ID")),
            lo_freq = settings.get("lo_freq"),
            lo_spectral_inversion = settings.get("lo_spectral_inversion"),
            if_range = settings.get("if_range"),
            total_sys_hours = int(settings.get("total_sys_hours")),
            internal_10MHz = int(settings.get("internal_10MHz")),
            supply_voltage_min = int(settings.get("supply_voltage_min")),
            supply_voltage_max = int(settings.get("supply_voltage_max")),
        )
        return factoryIBUC_data

    except Exception as e:
        print(f"Error loading config from {file_path}: {e}")
        return None


FactoryIBUCData0 = load_factory_ibuc_cfg("ibuc_A_factory.conf");
print(FactoryIBUCData0);

FactoryIBUCData1 = load_factory_ibuc_cfg("ibuc_B_factory.conf");
print(FactoryIBUCData1);

FactoryIBUCData2 = load_factory_ibuc_cfg("ibuc_SPARE_factory.conf");
print(FactoryIBUCData2);