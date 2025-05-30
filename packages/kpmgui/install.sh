#!/bin/sh
set -eu

mkdir /mnt/us/extensions/kpm
touch /mnt/us/extensions/kpm/extension.py

cat > /mnt/us/extensions/kpm/extension.py << 'EOF'
#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# KPM Manager for KUAL
#

# KUAL imports
from KUAL import golden, ui, utils

import subprocess

class KPMApp(golden.App):
    def __init__(self):
        # id, title, description
        super(KPMApp, self).__init__('kpm', 'KPM Manager', 'Install/Remove/Query packages')

    def run(self):
        # Main menu
        choices = ['Install', 'Remove', 'Query', 'Configure', 'Quit']
        idx = ui.Menu('KPM Manager', choices).show()
        if idx < 0 or choices[idx] == 'Quit':
            return  # exit to KUAL

        action = choices[idx]
        pkg = ui.TextInput('Package name').show().strip()
        if not pkg:
            ui.InfoDialog('No package entered').show()
            return self.run()

        if action == 'Install':
            cmd = ['kpm', '-S', pkg]
        elif action == 'Remove':
            cmd = ['kpm', '-R', pkg]
        elif action == 'Query':
            # ask remote vs local
            sub = ui.Menu('Query type', ['Local','Remote']).show()
            cmd = ['kpm', '-Ql' if sub==0 else '-Qr', pkg]
        elif action == 'Configure':
            # open your config file in the Kindle’s text editor
            utils.exec_cmd(['vi', '/etc/kpm/kpm.conf'])
            return self.run()
        else:
            return

        # run the command, show a spinner
        spinner = ui.ProgressDialog('{} {}'.format(action, pkg), 'Please wait…')
        pid = spinner.start()
        try:
            out = subprocess.check_output(cmd, stderr=subprocess.STDOUT)
            spinner.finish()
            ui.TextBox(out.decode('utf-8')).show()
        except subprocess.CalledProcessError as e:
            spinner.finish()
            ui.TextBox(e.output.decode('utf-8') or 'Error code {}'.format(e.returncode)).show()

        # back to main menu
        return self.run()

# Tell KUAL about us
if __name__ == '__main__':
    KPMApp().run()
EOF

chmod +x /mnt/us/extensions/kpm/extension.py