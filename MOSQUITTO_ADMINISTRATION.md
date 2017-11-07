# HOW SET USERNAME AND PASSOWORD

I you want disallow anonimous publish/subscribe and use username/password authentication, then

http://stackoverflow.com/questions/31156817/how-to-set-username-password-mosquitto


set in `/etc/mosquitto/mosquitto.conf`

`allow_anomymous false`

In code after function `mosquitto_new`

```
// username/password "admin"/admin
mosquitto_username_pw_set(mosq, "admin", "admin");

```


----------------------------------------------------

# SHOW STATUS SERVICE

`sudo systemctl status mosquitto.service`

# RESTART SERVICE

`sudo systemctl restart mosquitto.service`

s 