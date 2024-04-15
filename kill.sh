#!/bin/sh

var=$(pgrep uchat_server)

kill $var

if [ -n "$var" ]; then
    echo "Умер мужик" >> server.log
fi