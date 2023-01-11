import os
import subprocess
import sys
import requests
from esptool import ESPLoader
import shutil
import random
import string
import json
import subprocess

def get_random_string(length):
    # choose from all lowercase letter
    letters = string.ascii_lowercase
    result_str = ''.join(random.choice(letters) for i in range(length))
    return result_str
    

class DoMOActuatorFlasher:

    def __init__(self, serial_port, device_type):
        self.device_type = device_type
        self.serial_port = serial_port

    def get_mac_address(self):
        result = subprocess.run(["esptool.py", "chip_id"], stdout=subprocess.PIPE).stdout.decode('utf-8')
        print(result)
        res = result.split('\n')
        mac = ""
        for r in res:
            if r.find("MAC") != -1:
                r = r.replace("MAC", "")
                r = r.replace(":", "")
                mac = r
        return mac

    def run_flasher_esp32(self):
        cmd = "esptool.py --chip esp32 --port \"/dev/ttyUSB0\" --baud 460800 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 40m --flash_size 4MB 0x1000 /root/.platformio/packages/framework-arduino-solo1/tools/sdk/esp32/bin/bootloader_dio_40m.bin 0x8000 .pio/build/shelly1plus/partitions.bin 0xe000 /root/.platformio/packages/framework-arduino-solo1/tools/partitions/boot_app0.bin 0x10000 .pio/build/shelly1plus/firmware.bin"
        print("Executing %s" % cmd)
        p = subprocess.Popen(cmd,
                             bufsize=1, shell=True, stderr=sys.stderr, stdout=sys.stdout)
        p.wait()
        return p.returncode == 0

    def run_flasher(self):
        cmd = "pio run -e " + self.device_type + " -t nobuild --target=upload"
        print("Executing %s" % cmd)
        p = subprocess.Popen(cmd,
                             bufsize=1, shell=True, stderr=sys.stderr, stdout=sys.stdout)
        p.wait()
        return p.returncode == 0
    
    def run_erase(self):
        cmd = "pio run -e " + self.device_type + " --target=erase"
        print("Executing %s" % cmd)
        p = subprocess.Popen(cmd,
                             bufsize=1, shell=True, stderr=sys.stderr, stdout=sys.stdout)
        p.wait()
        return p.returncode == 0
    
    def run_uploadfs(self):
        cmd = "pio run -e " + self.device_type + " --target=uploadfs"
        print("Executing %s" % cmd)
        p = subprocess.Popen(cmd,
                             bufsize=1, shell=True, stderr=sys.stderr, stdout=sys.stdout)
        p.wait()
        return p.returncode == 0
    
    
    
    def generate_keys(self):
        print("Generate keys ...")

        model = "shelly_" + self.device_type.replace("shelly", "")

        cmd = "./generateSecMaterial.sh " + model + " " + self.get_mac_address()
        print("Executing %s" % cmd)
        p = subprocess.Popen(cmd,
                             bufsize=1, shell=True, stderr=sys.stderr, stdout=sys.stdout)
        p.wait()
        return p.returncode == 0

    def generate_sec_json_file_shelly1plus(self):
        cert = ""

        with open('/work_dir/ca-domo.crt', 'r') as file:
            cert = file.read()

        user_login = get_random_string(8)
        user_password = get_random_string(8)

        to_write = {
           "caCert": cert,
           "authUser": user_login,
           "authPassword": user_password
        }

        with open('./data/sec.json', 'w') as f:
            json.dump(to_write, f)

        return (user_login, user_password)


    
    def generate_sec_json_file(self):
        cert = ""
        key = ""
        
        with open('./data/Cert.pem', 'r') as file:
            cert = file.read()
        
        with open('./data/Key.pem', 'r') as file:
            key = file.read()
        
        user_login = get_random_string(8)
        user_password = get_random_string(8)
        
        to_write = {
           "serverCert": cert,
           "serverKey": key,
           "authUser": user_login,
           "authPassword": user_password
        }    
        
        with open('./data/sec.json', 'w') as f:
            json.dump(to_write, f)
        
        os.remove('./data/Shelly.csr')
        os.remove('./data/Key.pem')
        os.remove('./data/Cert.pem')
        
        return (user_login, user_password)
        
        
        

DEVICE_TYPES = {
    1: 'shelly25',
    2: 'shelly1pm',
    3: 'shelly1',
    4: 'shellydimmer',
    5: 'shellyem',
    6: 'shellyrgbw',
    7: 'shelly1plus'
}

found = False
DEVICE_TYPE = ''

while not found:
    print("\n\nSupported devices: ")
    for i, name in DEVICE_TYPES.items():
        print("%s: %s" % (i, name))
    try:
        selected = int(input("\nSelect type of device to flash: "))
    except Exception as ex:
        continue

    if selected in DEVICE_TYPES:
        DEVICE_TYPE = DEVICE_TYPES[selected]
        found = True

print("Selected type: %s" % DEVICE_TYPE)

PORT = "/dev/ttyUSB0"
if len(sys.argv) > 1:
    PORT = sys.argv[1]

while True:
    input("Press a button to proceed ... ")

    if os.path.isdir('data'):
        shutil.rmtree("./data")
    try:
        os.remove("./spiffs.bin")
    except OSError:
        pass
    
    os.mkdir("./data")     
    
   
    print("Created new data directory")
    
    if DEVICE_TYPE == 'shellydimmer':
        print("Copy stm32.bin")
        shutil.copy("./stm32.bin", "./data/")
        

    ok = False
    mac = ""

    while not ok:
        try:
            m = DoMOActuatorFlasher(PORT, DEVICE_TYPE)
            mac = ""
            while mac == "":
                mac = m.get_mac_address()
                if mac == "":
                    input("Reset board, disconnect and connect again Vcc cable  ... and press a button")
            print("MAC Address: " + mac)
            if DEVICE_TYPE != "shelly1plus":
                ok = m.generate_keys()
                assert ok
                (user_login, user_password) = m.generate_sec_json_file()
            if DEVICE_TYPE == "shelly1plus":
                (user_login, user_password) = m.generate_sec_json_file_shelly1plus()
            input("Reset board, disconnect and connect again Vcc cable  ... and press a button")
            print("Erasing flash")
            ok = m.run_erase()
            assert ok
            input("Reset board, disconnect and connect again Vcc cable  ... and press a button")
            print("Uploading sec material")
            ok = m.run_uploadfs()
            input("Reset board, disconnect and connect again Vcc cable  ... and press a button")
            print("Flashing program")
            shutil.copy("/fw_binaries/" + DEVICE_TYPE + ".bin", "/work_dir/.pio/build/" + DEVICE_TYPE + "/firmware.bin")
            if DEVICE_TYPE != "shelly1plus":
                ok = m.run_flasher()
                assert ok
            if DEVICE_TYPE == "shelly1plus":
                shutil.copy("/fw_binaries/partitions.bin", "/work_dir/.pio/build/shelly1plus/partitions.bin")
                ok = m.run_flasher_esp32()
                assert ok
            with open("/out/out.txt", "a") as file_object:
                file_object.write(DEVICE_TYPE + "\t" + mac + "\t" + user_login + "\t" + user_password + "\n")
        except Exception as ex:
            print(ex)
            input("Press a button to retry ...")
