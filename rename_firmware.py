Import("env")
import shutil
import os

def post_build_rename(source, target, env):
    # Hole den Build-Pfad und Environment-Namen
    firmware_source = str(target[0])
    env_name = env["PIOENV"]
    
    # Definiere die Ziel-Dateinamen basierend auf dem Environment
    filename_map = {
        # NTP builds
        "deutsche_11x11": "firmware_deutsche_11x11.bin",
        "bayrisch_11x11": "firmware_bayrisch_11x11.bin",
        "mini_8x8": "firmware_mini_8x8.bin",
        # RCT/RTC builds (some configs use _rct, some _rtc)
        "deutsche_11x11_rct": "firmware_deutsche_11x11_rct.bin",
        "bayrisch_11x11_rct": "firmware_bayrisch_11x11_rct.bin",
        "mini_8x8_rct": "firmware_mini_8x8_rct.bin",
        # aliases in case env names use 'rtc' spelling
        "deutsche_11x11_rtc": "firmware_deutsche_11x11_rct.bin",
        "bayrisch_11x11_rtc": "firmware_bayrisch_11x11_rct.bin",
        "mini_8x8_rtc": "firmware_mini_8x8_rct.bin"
    }
    
    if env_name in filename_map:
        # Erstelle den neuen Dateinamen im gleichen Verzeichnis
        build_dir = os.path.dirname(firmware_source)
        new_firmware_path = os.path.join(build_dir, filename_map[env_name])
        
        # Kopiere die Datei mit neuem Namen
        shutil.copy(firmware_source, new_firmware_path)
        print(f"\n*** Firmware kopiert nach: {new_firmware_path} ***\n")
        
        # Optional: Kopiere auch ins Projekt-Root für einfachen Zugriff
        project_root = env["PROJECT_DIR"]
        root_firmware_path = os.path.join(project_root, filename_map[env_name])
        shutil.copy(firmware_source, root_firmware_path)
        print(f"*** Firmware auch kopiert nach Projekt-Root: {root_firmware_path} ***\n")

# Registriere das Post-Build-Script
env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", post_build_rename)
