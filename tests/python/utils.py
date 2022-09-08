from pathlib import Path

def app_path_from_app_name(app_dir, app_name: str, device: str) -> Path:
    assert app_dir.is_dir(), f"{app_dir} is not a directory"
    app_path = app_dir / (app_name + "_" + device + ".elf")
    assert app_path.is_file(), f"{app_path} must exist"
    return app_path

def concatenate(*args):
    result = b''
    for arg in args:
        result += (bytes([len(arg)]) + arg)
    return result

BIP32_HARDENED_BIT = ((1 << 31) >> 0)
def bip32_harden(n: int) -> int:
    return (n | BIP32_HARDENED_BIT) >> 0
