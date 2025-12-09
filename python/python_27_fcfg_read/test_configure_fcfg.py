import pytest

from fcfg import FactoryCntrlrData, configure_fcfg, mnm, MAX_STR_LEN

def make_base():
    """Helper: make a dummy FactoryCntrlrData with placeholders."""
    return FactoryCntrlrData(
        serial_number="Default",
        model_number="Default",
        private_label="Default",
        hardware_revision="Default",
        total_sys_hours=0,
        internal_10MHz=0,
        supply_voltage_min=0,
        supply_voltage_max=0,
    )

def test_default_model():
    f = make_base()
    f.model_number = "Default"
    out = configure_fcfg(f)
    assert out.internal_10MHz == 0
    assert (out.supply_voltage_min, out.supply_voltage_max) == tuple(mnm["AC_range"])

@pytest.mark.parametrize("code,expected_mhz,expected_range", [
    ("TXI-21NAW-000", mnm["ten_MHz_opt"][1], mnm["AC_range"]),  # ten_MHz_opt = 1, supply A
    ("TXI-22NAW-000", mnm["ten_MHz_opt"][2], mnm["AC_range"]),  # ten_MHz_opt = 2, supply D
])
def test_valid_model_codes(code, expected_mhz, expected_range):
    f = make_base()
    # Build a 6-char code: 3 for prefix, 1 for rdd, 1 for ten_MHz, 1 for supply
    # e.g. "TXI10A"
    f.model_number = code
    out = configure_fcfg(f)
    assert out.internal_10MHz == expected_mhz
    assert (out.supply_voltage_min, out.supply_voltage_max) == tuple(expected_range)

@pytest.mark.parametrize("bad_code", [
    "TOO_LONG_" + "X" * (MAX_STR_LEN),    # exceeds max length
    "XXX00A",   # wrong prefix
    "TXI99A",   # invalid rdd_opt
    "TXI1X A",  # invalid ten_MHz or space
    "TXI10Z",   # invalid supply_opt
])
def test_invalid_model_codes_fall_back_to_default(bad_code):
    f = make_base()
    f.model_number = bad_code
    out = configure_fcfg(f)
    # on invalid, should reset to AC defaults
    assert out.internal_10MHz == 0
    assert (out.supply_voltage_min, out.supply_voltage_max) == tuple(mnm["AC_range"])
