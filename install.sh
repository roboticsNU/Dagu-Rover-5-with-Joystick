!#/bin/bash
cp joystickmachine.service /lib/systemd/joystickmachine.service
cd /etc/systemd/system/
ln /lib/systemd/joystickmachine.service joystickmachine.service

systemctl daemon-reload
systemctl start joystickmachine.service
systemctl enable joystickmachine.service
