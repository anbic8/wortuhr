Import("env")
import shutil
import os

def post_build_rename(source, target, env):
    # Hole den Build-Pfad und Environment-Namen
    firmware_source = str(target[0])
    env_name = env["PIOENV"]

    # Zielname folgt immer demselben Schema wie der Environment-Name -
    # kein manuell gepflegtes Mapping noetig, das bei jeder neuen
    # Environment in platformio.ini sonst extra ergaenzt werden muesste.
    filename = f"firmware_{env_name}.bin"

    # Erstelle den neuen Dateinamen im gleichen Verzeichnis
    build_dir = os.path.dirname(firmware_source)
    new_firmware_path = os.path.join(build_dir, filename)

    # Kopiere die Datei mit neuem Namen
    shutil.copy(firmware_source, new_firmware_path)
    print(f"\n*** Firmware kopiert nach: {new_firmware_path} ***\n")

    # Optional: Kopiere auch ins Projekt-Root für einfachen Zugriff
    project_root = env["PROJECT_DIR"]
    root_firmware_path = os.path.join(project_root, filename)
    shutil.copy(firmware_source, root_firmware_path)
    print(f"*** Firmware auch kopiert nach Projekt-Root: {root_firmware_path} ***\n")

# Registriere das Post-Build-Script
env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", post_build_rename)
