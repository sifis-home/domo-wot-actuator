#!/bin/bash
source /root/.platformio/penv/bin/activate
pip3 install -r /work_dir/requirements.txt
python3 flash_device.py
